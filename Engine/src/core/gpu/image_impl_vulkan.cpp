#include "image_impl_vulkan.h"
#include "buffer_impl_vulkan.h"
#include "commandbuffer_impl_vulkan.h"

#include "../graphicscontext_impl_vulkan.h"
#include "../gpu_detail/vk_initializers.h"
#include "../gpu_detail/vk_converters.h"

using namespace vde::core::gpu;

VkSampler s_CreateSampler(VkDevice device)
{
    VkSamplerCreateInfo sampler_create_info { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
    sampler_create_info.addressModeU     = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_create_info.addressModeV     = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_create_info.addressModeW     = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    sampler_create_info.minFilter        = VK_FILTER_LINEAR;
    sampler_create_info.magFilter        = VK_FILTER_LINEAR;
    sampler_create_info.mipmapMode       = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_create_info.anisotropyEnable = true;
    sampler_create_info.maxAnisotropy    = 4.0f;

    VkSampler result = VK_NULL_HANDLE;
    vkCreateSampler(device, &sampler_create_info, nullptr, &result);
    return result;
}

Image::Image()
	: m_pImpl(new Impl)
	, m_owner(nullptr)
{
}

Image::Image(util::Badge<core::GraphicsContext> badge, const Info& info)
    : m_pImpl(new Impl)
    , m_owner(badge.Owner())
{
    VkImageUsageFlags imageUsages = s_ToVk(info.usageBits);
    
    if (info.initialData)
        imageUsages = imageUsages | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    m_pImpl->usage                = info.usageBits;
    m_pImpl->format               = s_ToVk(info.format);
    m_pImpl->extent               = { info.extent.x, info.extent.y, info.extent.z };
    m_pImpl->mipCount             = info.mipCount;
    m_pImpl->layerCount           = info.layerCount;

    auto image_create_info = gpu_detail::image_create_info(m_pImpl->format, imageUsages, m_pImpl->extent, info.type, info.mipCount, info.layerCount);

    VmaAllocationCreateInfo image_alloc_create_info {
        .usage         = VMA_MEMORY_USAGE_GPU_ONLY, // FIXME: Might need to be changed to suit data transfers
        .requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };
    TRACE_VMA_ALLOC(m_owner->GetImpl().device);
    vmaCreateImage(m_owner->GetImpl().allocator, &image_create_info, &image_alloc_create_info, &m_pImpl->image, &m_pImpl->allocation, nullptr);

    VkImageAspectFlags aspectFlags = 0;
    if (info.format.first == EImageFormatComponents::Depth)
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
    else if (info.format.first == EImageFormatComponents::DepthStencil)
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    else if (info.format.first == EImageFormatComponents::Stencil)
        aspectFlags = VK_IMAGE_ASPECT_STENCIL_BIT;
    else
        aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

    auto imageview_create_info = gpu_detail::imageview_create_info(m_pImpl->format, m_pImpl->image, aspectFlags, info.type, info.mipCount, info.layerCount);
    vkCreateImageView(m_owner->GetImpl().device, &imageview_create_info, nullptr, &m_pImpl->imageView);

    m_pImpl->imageSampler = s_CreateSampler(m_owner->GetImpl().device);

    if (info.initialData)
        Upload(glm::uvec3(0), info.extent, info.initialData);
}

Image::Image(util::Badge<core::GraphicsContext> badge, const PreexistingImageInfo& info)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
{
	m_pImpl->image     = info.image;
	m_pImpl->imageView = info.imageView;
    m_pImpl->format    = info.format;
    m_pImpl->extent    = info.extent;

    m_pImpl->imageSampler = s_CreateSampler(m_owner->GetImpl().device);
}

Image::~Image() noexcept
{
    if (m_owner)
    {
        if (m_pImpl->imageSampler != VK_NULL_HANDLE)
            vkDestroySampler(m_owner->GetImpl().device, m_pImpl->imageSampler, nullptr);

        if (m_pImpl->imageView != VK_NULL_HANDLE)
            vkDestroyImageView(m_owner->GetImpl().device, m_pImpl->imageView, nullptr);

        if (m_pImpl->allocation)
        {
            TRACE_VMA_FREE(m_owner->GetImpl().device);
            vmaDestroyImage(m_owner->GetImpl().allocator, m_pImpl->image, m_pImpl->allocation);
        }
    }
}

Image::Impl& Image::GetImpl()
{
	return *m_pImpl;
}

glm::uvec3 Image::Size() const
{
    return glm::uvec3(m_pImpl->extent.width, m_pImpl->extent.height, m_pImpl->extent.depth);
}

size_t Image::ArraySize() const
{
    return m_pImpl->layerCount;
}

ImageFormat Image::Format() const
{
    return s_ToVde(m_pImpl->format);
}

void Image::Upload(const glm::vec3& offset, const glm::uvec3& size, const void* pixels)
{
    size_t dataSize = size.x * size.y * size.z * s_ToVkPixelSize(m_pImpl->format);

    if (auto stagingBuffer = m_owner->CreateBuffer({ .sizeInBytes = dataSize, .usage = EBufferUsageBits::TransferSrc }); stagingBuffer)
    {
        std::copy_n((const std::byte*)pixels, dataSize, (std::byte*)stagingBuffer->GetImpl().allocationInfo.pMappedData);

        auto& cb = m_owner->CommandPool().Acquire();
        if (auto encoder = cb.Record("Image upload", ECommandBufferRecordType::OneTimeSubmit); encoder)
        {
            encoder->TransitionImage(*this, EImageLayout::TransferDst);

            VkBufferImageCopy copyRegion {};
            copyRegion.bufferOffset      = 0;
            copyRegion.bufferRowLength   = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel       = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount     = 1;
            copyRegion.imageExtent                     = VkExtent3D(size.x, size.y, size.z);

            vkCmdCopyBufferToImage(cb.GetImpl().cmd, stagingBuffer->GetImpl().buffer, m_pImpl->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            encoder->TransitionImage(*this, EImageLayout::ShaderReadOnly);
        }

        m_owner->GetImpl().SubmitInternal(cb.GetImpl().cmd, {}, {}, cb.GetImpl().fence);
        cb.WaitForSignal();

        m_owner->CommandPool().Release(cb);
    }
}
