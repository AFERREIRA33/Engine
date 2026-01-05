#include "commandpool_impl_vulkan.h"
#include "commandbuffer_impl_vulkan.h"
#include "../graphicscontext_impl_vulkan.h"

#include "../gpu_detail/vk_initializers.h"

using namespace vde::core::gpu;

CommandPool::CommandPool(util::Badge<core::GraphicsContext> badge)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
{
	auto create_info = gpu_detail::command_pool_create_info(m_owner->GetImpl().graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	vkCreateCommandPool(m_owner->GetImpl().device, &create_info, nullptr, &m_pImpl->pool);
}

CommandPool::~CommandPool() noexcept
{
	for (auto& [cb, _] : m_commandBuffers)
		delete cb;

	vkDestroyCommandPool(m_owner->GetImpl().device, m_pImpl->pool, nullptr);
}

CommandPool::Impl& CommandPool::GetImpl()
{
	return *m_pImpl;
}

vde::core::GraphicsContext& CommandPool::Owner()
{
	return *m_owner;
}

CommandBuffer& CommandPool::Acquire(bool isSecondary /* = false */)
{
	if (isSecondary)
	{
		for (auto& [cb, available] : m_secondaryCommandBuffers)
		{
			if (available && cb->IsSignaled())
			{
				available = false;
				return *cb;
			}
		}

		auto newCb = new CommandBuffer(vME, true);
		m_secondaryCommandBuffers[newCb] = false;
		return *newCb;
	}

	for (auto& [cb, available] : m_commandBuffers)
	{
		if (available && cb->IsSignaled())
		{
			available = false;
			return *cb;
		}
	}

	auto newCb = new CommandBuffer(vME);
	m_commandBuffers[newCb] = false;
	return *newCb;
}

void CommandPool::Release(CommandBuffer& cb)
{
	auto ptr = &cb;
	if (m_secondaryCommandBuffers.contains(ptr))
		m_secondaryCommandBuffers[&cb] = true;
	else
		m_commandBuffers[&cb] = true;
}

void CommandPool::Purge()
{
	std::vector<CommandBuffer*> toDelete;

	for (auto& [cb, available] : m_commandBuffers)
		if (available && cb->IsSignaled())
			toDelete.push_back(cb);
	
	for (auto& cb : toDelete)
	{
		m_commandBuffers.erase(cb);
		delete cb;
	}

	toDelete.clear();

	for (auto& [cb, available] : m_secondaryCommandBuffers)
		if (available && cb->IsSignaled())
			toDelete.push_back(cb);

	for (auto& cb : toDelete)
	{
		m_secondaryCommandBuffers.erase(cb);
		delete cb;
	}
}