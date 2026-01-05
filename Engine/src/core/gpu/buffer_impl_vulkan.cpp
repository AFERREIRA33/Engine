#include "buffer_impl_vulkan.h"
#include "commandbuffer_impl_vulkan.h"
#include "commandpool_impl_vulkan.h"
#include "../graphicscontext_impl_vulkan.h"
#include "../gpu_detail/vk_initializers.h"

using namespace vde::core::gpu;

Buffer::Buffer(util::Badge<core::GraphicsContext> badge, const Info& info)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
{
	VkBufferCreateInfo createInfo { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	createInfo.size  = info.sizeInBytes;
	createInfo.usage = VkBufferUsageFlagBits(info.usage);

	VmaAllocationCreateInfo allocInfo {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	TRACE_VMA_ALLOC(m_owner->GetImpl().device);
	vmaCreateBuffer(m_owner->GetImpl().allocator, &createInfo, &allocInfo, &m_pImpl->buffer, &m_pImpl->allocation, &m_pImpl->allocationInfo);
	m_pImpl->size  = info.sizeInBytes;
	m_pImpl->usage = info.usage;

	if (info.initialData)
		Upload(info.initialData, info.sizeInBytes);
}

Buffer::~Buffer() noexcept
{
	TRACE_VMA_FREE(m_owner->GetImpl().device);
	vmaDestroyBuffer(m_owner->GetImpl().allocator, m_pImpl->buffer, m_pImpl->allocation);
}

Buffer::Impl& Buffer::GetImpl()
{
	return *m_pImpl;
}

size_t Buffer::Size() const
{
	return m_pImpl->size;
}

void Buffer::Upload(const void* data, size_t size)
{
	if (auto stagingBuffer = m_owner->CreateBuffer({ .sizeInBytes = size, .usage = EBufferUsageBits::TransferSrc }); stagingBuffer)
	{
		std::copy_n((const std::byte*)data, size, (std::byte*)stagingBuffer->GetImpl().allocationInfo.pMappedData);
	
		auto& cb = m_owner->CommandPool().Acquire();
		if (auto encoder = cb.Record("Buffer upload", ECommandBufferRecordType::OneTimeSubmit); encoder)
		{
			VkBufferCopy copyInfo {};
			copyInfo.srcOffset = 0;
			copyInfo.dstOffset = 0;
			copyInfo.size = VkDeviceSize(size);
			vkCmdCopyBuffer(cb.GetImpl().cmd, stagingBuffer->GetImpl().buffer, m_pImpl->buffer, 1, &copyInfo);
		}

		m_owner->GetImpl().SubmitInternal(cb.GetImpl().cmd, {}, {}, cb.GetImpl().fence);
		cb.WaitForSignal();

		m_owner->CommandPool().Release(cb);
	}
}

void Buffer::Upload(const ShaderDataStore& dataStore)
{
	Upload(dataStore.Data(), dataStore.Size());
}
