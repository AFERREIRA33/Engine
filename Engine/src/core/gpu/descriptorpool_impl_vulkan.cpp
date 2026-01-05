#include "descriptorpool_impl_vulkan.h"
#include "descriptorset_impl_vulkan.h"
#include "../graphicscontext_impl_vulkan.h"

using namespace vde::core::gpu;

DescriptorPool::DescriptorPool(util::Badge<core::GraphicsContext> badge)
	: m_owner(badge.Owner())
	, m_pImpl(new Impl)
{
	std::vector<VkDescriptorPoolSize> poolSizes = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1024 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1024 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2048 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1024 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,           512 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,    256 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,    256 },
		{ VK_DESCRIPTOR_TYPE_SAMPLER,                 512 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,        256 }
	};

	VkDescriptorPoolCreateInfo poolInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets       = 2048;
	poolInfo.poolSizeCount = uint32_t(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();

	vkCreateDescriptorPool(m_owner->GetImpl().device, &poolInfo, nullptr, &m_pImpl->pool);
}

DescriptorPool::~DescriptorPool() noexcept
{
	vkDestroyDescriptorPool(m_owner->GetImpl().device, m_pImpl->pool, nullptr);
}

DescriptorPool::Impl& DescriptorPool::GetImpl()
{
	return *m_pImpl;
}

vde::core::GraphicsContext& DescriptorPool::Owner()
{
	return *m_owner;
}

std::unique_ptr<DescriptorSet> DescriptorPool::Acquire(const DescriptorSetLayout& layout)
{
	return std::make_unique<DescriptorSet>(vME, layout);
}
