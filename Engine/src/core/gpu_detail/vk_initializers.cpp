#include "vk_initializers.h"
#include "vk_converters.h"
#include "../graphicscontext_impl_vulkan.h"

// Heavily inspired by https://github.com/vblanco20-1/vulkan-guide/tree/starting-point-2

VkCommandPoolCreateInfo vde::core::gpu_detail::command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags /*= 0*/)
{
    VkCommandPoolCreateInfo info = { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    info.queueFamilyIndex = queueFamilyIndex;
    info.flags            = flags;
    return info;
}

VkCommandBufferAllocateInfo vde::core::gpu_detail::command_buffer_allocate_info(VkCommandPool pool, bool isSecondary /* = false */, uint32_t count /*= 1*/)
{
    VkCommandBufferAllocateInfo info = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    info.commandPool        = pool;
    info.commandBufferCount = count;
    info.level              = isSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    return info;
}

VkCommandBufferBeginInfo vde::core::gpu_detail::command_buffer_begin_info(VkCommandBufferUsageFlags flags /*= 0*/)
{
    VkCommandBufferBeginInfo info = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO  };
    info.pInheritanceInfo = nullptr;
    info.flags            = flags;
    return info;
}

VkFenceCreateInfo vde::core::gpu_detail::fence_create_info(VkFenceCreateFlags flags /*= 0*/)
{
    VkFenceCreateInfo info = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    info.flags = flags;
    return info;
}

VkSemaphoreCreateInfo vde::core::gpu_detail::semaphore_create_info(VkSemaphoreCreateFlags flags /*= 0*/)
{
    return VkSemaphoreCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .flags = flags
    };
}

VkSemaphoreSubmitInfo vde::core::gpu_detail::semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
    VkSemaphoreSubmitInfo submitInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
    submitInfo.semaphore   = semaphore;
    submitInfo.stageMask   = stageMask;
    submitInfo.deviceIndex = 0;
    submitInfo.value       = 1;

    return submitInfo;
}

VkCommandBufferSubmitInfo vde::core::gpu_detail::command_buffer_submit_info(VkCommandBuffer cmd)
{
    VkCommandBufferSubmitInfo info{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
    info.commandBuffer = cmd;
    info.deviceMask    = 0;

    return info;
}

VkSubmitInfo2 vde::core::gpu_detail::submit_info(VkCommandBufferSubmitInfo* cmd, const std::vector<VkSemaphoreSubmitInfo>& signalSemaphoreInfos /*= {}*/, const std::vector<VkSemaphoreSubmitInfo>& waitSemaphoreInfos /*= {}*/)
{
    VkSubmitInfo2 info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
    info.waitSemaphoreInfoCount = uint32_t(waitSemaphoreInfos.size());
    info.pWaitSemaphoreInfos    = info.waitSemaphoreInfoCount ? waitSemaphoreInfos.data() : nullptr;

    info.signalSemaphoreInfoCount = uint32_t(signalSemaphoreInfos.size());
    info.pSignalSemaphoreInfos    = info.signalSemaphoreInfoCount ? signalSemaphoreInfos.data() : nullptr;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos    = cmd;

    return info;
}

VkPresentInfoKHR vde::core::gpu_detail::present_info(const VkSwapchainKHR& swapchain,const VkSemaphore& waitSemaphore, const uint32_t& imageIndex)
{
    VkPresentInfoKHR info = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };

    info.pSwapchains = &swapchain;
    info.swapchainCount = 1;

    info.pWaitSemaphores = &waitSemaphore;
    info.waitSemaphoreCount = 1;

    info.pImageIndices = &imageIndex;

    return info;
}

VkRenderingAttachmentInfo vde::core::gpu_detail::attachment_info(VkImageView view, VkClearValue* clear, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    VkRenderingAttachmentInfo colorAttachment{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
    colorAttachment.imageView   = view;
    colorAttachment.imageLayout = layout;
    colorAttachment.loadOp      = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;

    if (clear)
        colorAttachment.clearValue = *clear;

    return colorAttachment;
}

VkRenderingAttachmentInfo vde::core::gpu_detail::depth_attachment_info( VkImageView view, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/)
{
    VkRenderingAttachmentInfo depthAttachment{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
    depthAttachment.imageView                     = view;
    depthAttachment.imageLayout                   = layout;
    depthAttachment.loadOp                        = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp                       = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.clearValue.depthStencil.depth = 0.0f;

    return depthAttachment;
}

VkRenderingInfo vde::core::gpu_detail::rendering_info(VkExtent2D renderExtent, const std::vector<VkRenderingAttachmentInfo>& colorAttachments, VkRenderingAttachmentInfo* depthAttachment)
{
    VkRenderingInfo renderInfo{ .sType = VK_STRUCTURE_TYPE_RENDERING_INFO };
    renderInfo.renderArea           = VkRect2D{ VkOffset2D { 0, 0 }, renderExtent };
    renderInfo.layerCount           = 1;
    renderInfo.colorAttachmentCount = uint32_t(colorAttachments.size());
    renderInfo.pColorAttachments    = colorAttachments.data();
    renderInfo.pDepthAttachment     = depthAttachment;
    renderInfo.pStencilAttachment   = nullptr;

    return renderInfo;
}

VkImageSubresourceRange vde::core::gpu_detail::image_subresource_range(VkImageAspectFlags aspectMask)
{
    VkImageSubresourceRange subImage{};
    subImage.aspectMask     = aspectMask;
    subImage.baseMipLevel   = 0;
    subImage.levelCount     = VK_REMAINING_MIP_LEVELS;
    subImage.baseArrayLayer = 0;
    subImage.layerCount     = VK_REMAINING_ARRAY_LAYERS;

    return subImage;
}

VkShaderModuleCreateInfo vde::core::gpu_detail::shader_module_create_info(const void* data, size_t size)
{
    VkShaderModuleCreateInfo result { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    result.pCode    = reinterpret_cast<const uint32_t*>(data);
    result.codeSize = size;
    return result;
}

VkDescriptorSetLayoutBinding vde::core::gpu_detail::descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
    VkDescriptorSetLayoutBinding setbind = {};
    setbind.binding            = binding;
    setbind.descriptorCount    = 1;
    setbind.descriptorType     = type;
    setbind.pImmutableSamplers = nullptr;
    setbind.stageFlags         = stageFlags;

    return setbind;
}

VkDescriptorSetLayoutCreateInfo vde::core::gpu_detail::descriptorset_layout_create_info(VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount)
{
    VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    info.pBindings    = bindings;
    info.bindingCount = bindingCount;
    info.flags        = 0;

    return info;
}

VkWriteDescriptorSet vde::core::gpu_detail::write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding)
{
    VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write.dstBinding      = binding;
    write.dstSet          = dstSet;
    write.descriptorCount = 1;
    write.descriptorType  = type;
    write.pImageInfo      = imageInfo;

    return write;
}

VkWriteDescriptorSet vde::core::gpu_detail::write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding)
{
    VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write.dstBinding      = binding;
    write.dstSet          = dstSet;
    write.descriptorCount = 1;
    write.descriptorType  = type;
    write.pBufferInfo     = bufferInfo;

    return write;
}

VkDescriptorBufferInfo vde::core::gpu_detail::buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
    VkDescriptorBufferInfo binfo = {};
    binfo.buffer = buffer;
    binfo.offset = offset;
    binfo.range  = range;
    return binfo;
}

VkImageCreateInfo vde::core::gpu_detail::image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent, vde::core::gpu::EImageType imageType, uint32_t mipLevels /* = 1 */, uint32_t arrayLayers /* = 1 */)
{
    auto vkType = s_ToVk(imageType, arrayLayers);

    VkImageCreateInfo info { .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    info.imageType   = std::get<0>(vkType);
    info.flags       = std::get<2>(vkType);
    info.format      = format;
    info.extent      = extent;
    info.mipLevels   = mipLevels;
    info.arrayLayers = arrayLayers;
    info.tiling      = VK_IMAGE_TILING_OPTIMAL;
    info.samples     = VK_SAMPLE_COUNT_1_BIT;
    info.usage       = usageFlags;

    return info;
}

VkImageViewCreateInfo vde::core::gpu_detail::imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags, vde::core::gpu::EImageType imageType, uint32_t mipLevels /* = 1 */, uint32_t arrayLayers /* = 1 */)
{
    auto vkType = s_ToVk(imageType, arrayLayers);

    // build a image-view for the depth image to use for rendering
    VkImageViewCreateInfo info { .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    info.viewType                        = std::get<1>(vkType);
    info.image                           = image;
    info.format                          = format;
    info.subresourceRange.baseMipLevel   = 0;
    info.subresourceRange.levelCount     = mipLevels;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount     = arrayLayers;
    info.subresourceRange.aspectMask     = aspectFlags;

    return info;
}

VkPipelineLayoutCreateInfo vde::core::gpu_detail::pipeline_layout_create_info()
{
    VkPipelineLayoutCreateInfo info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    return info;
}

VkPipelineShaderStageCreateInfo vde::core::gpu_detail::pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char* entry /* nullptr */)
{
    VkPipelineShaderStageCreateInfo info { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    info.stage  = stage;
    info.module = shaderModule;
    info.pName  = entry ? entry : "main";
    return info;
}

vde::core::gpu_detail::VkGraphicsPipelineBuilder& vde::core::gpu_detail::VkGraphicsPipelineBuilder::UseShaderStage(const VkPipelineShaderStageCreateInfo& stage)
{
    // TODO
    return *this;
}

VkPipeline vde::core::gpu_detail::VkGraphicsPipelineBuilder::Build(core::GraphicsContext& graphicsContext)
{
    VkGraphicsPipelineCreateInfo info { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    info.stageCount = uint32_t(m_shaderStages.size());
    info.pStages    = m_shaderStages.data();

    VkPipeline result = VK_NULL_HANDLE;
    vkCreateGraphicsPipelines(graphicsContext.GetImpl().device, VK_NULL_HANDLE, 1, &info, nullptr, &result);

    return result;
}
