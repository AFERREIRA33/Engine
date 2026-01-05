#define VMA_IMPLEMENTATION
#include "graphicscontext_impl_vulkan.h"
#include "window_impl_glfw_vulkan.h"

#include "gpu/commandbuffer_impl_vulkan.h"
#include "gpu/image_impl_vulkan.h"

#include "gpu_detail/vk_initializers.h"
#include "gpu_detail/vk_images.h"

#include <core/gpu/commandbuffer.h>
#include <core/gpu/shader.h>

using namespace vde::core;

auto& GraphicsContext::Impl::GetCurrentFrame()
{
	return perFrameData[currentFrame % 2];
};

void GraphicsContext::Impl::NewFrame()
{
	vkWaitForFences(device, 1, &GetCurrentFrame().commandBuffer->GetImpl().fence, true, 1000000000);
	vkResetFences(device, 1, &GetCurrentFrame().commandBuffer->GetImpl().fence);

	vkAcquireNextImageKHR(device, swapchain, 1000000000, GetCurrentFrame().swapchainSemaphore, nullptr, &swapchainImageIndex);
}

void GraphicsContext::Impl::SubmitInternal(
	VkCommandBuffer cmdBuffer,
	const std::vector<std::pair<VkPipelineStageFlagBits2, VkSemaphore>>& waitInfo,
	const std::vector<std::pair<VkPipelineStageFlagBits2, VkSemaphore>>& signalInfo,
	VkFence fence /* = VK_NULL_HANDLE */)
{
	std::vector<VkSemaphoreSubmitInfo> waitSemaphores(waitInfo.size(), {});
	for (size_t i = 0; i < waitSemaphores.size(); ++i)
		waitSemaphores[i] = gpu_detail::semaphore_submit_info(std::get<0>(waitInfo[i]), std::get<1>(waitInfo[i]));

	std::vector<VkSemaphoreSubmitInfo> signalSemaphores(signalInfo.size(), {});
	for (size_t i = 0; i < signalSemaphores.size(); ++i)
		signalSemaphores[i] = gpu_detail::semaphore_submit_info(std::get<0>(signalInfo[i]), std::get<1>(signalInfo[i]));

	auto commandBufferSubmitInfo = gpu_detail::command_buffer_submit_info(cmdBuffer);
	auto submitInfo = gpu_detail::submit_info(&commandBufferSubmitInfo, signalSemaphores, waitSemaphores);

	if (fence != VK_NULL_HANDLE)
		vkResetFences(device, 1, &fence);

	vkQueueSubmit2(graphicsQueue, 1, &submitInfo, fence);
}

GraphicsContext::GraphicsContext(Window& window)
	: m_pImpl(new Impl)
{
	auto windowImpl = window.GetImpl();
	m_pImpl->device = vkb::DeviceBuilder { windowImpl.physicalDevice }.build().value();

	m_pImpl->graphicsQueue       = m_pImpl->device.get_queue(vkb::QueueType::graphics).value();
	m_pImpl->graphicsQueueFamily = m_pImpl->device.get_queue_index(vkb::QueueType::graphics).value();

	VmaAllocatorCreateInfo allocator_create_info = {
		.flags          = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
		.physicalDevice = window.GetImpl().physicalDevice,
		.device         = m_pImpl->device,
		.instance       = window.GetImpl().instance
	};
	vmaCreateAllocator(&allocator_create_info, &m_pImpl->allocator);

	m_pImpl->swapchain = vkb::SwapchainBuilder{ m_pImpl->device }.set_desired_format(VkSurfaceFormatKHR{ .format = m_pImpl->swapchainImageFormat })
		                                                         .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		                                                         .set_desired_extent(window.GetDesc().size.x, window.GetDesc().size.y)
		                                                         .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	                                                             .build().value();

	m_pImpl->swapchainExtent = m_pImpl->swapchain.extent;

	auto swapchainImages     = m_pImpl->swapchain.get_images().value();
	auto swapchainImageViews = m_pImpl->swapchain.get_image_views().value();

	for (size_t i = 0; i < swapchainImages.size(); ++i)
		m_pImpl->swapchainImages.push_back(new gpu::Image(vME, gpu::PreexistingImageInfo { 
			swapchainImages[i],
			swapchainImageViews[i],
			m_pImpl->swapchainImageFormat,
			VkExtent3D { m_pImpl->swapchainExtent.width, m_pImpl->swapchainExtent.height, 1 }
		}));

	m_pImpl->backbuffer = std::make_unique<gpu::Image>(vME, gpu::Image::Info {
		.type      = gpu::EImageType::Image2D,
		.extent    = { m_pImpl->swapchainExtent.width, m_pImpl->swapchainExtent.height, 1 },
		.format    = std::make_pair(gpu::EImageFormatComponents::RGBA, gpu::EImageFormatType::F16),
		.usageBits = gpu::EImageUsageBits::TransferSrc | gpu::EImageUsageBits::TransferDst | gpu::EImageUsageBits::Storage | gpu::EImageUsageBits::ColorAttachment
	});

	m_pImpl->backbufferDepth = std::make_unique<gpu::Image>(vME, gpu::Image::Info {
		.type      = gpu::EImageType::Image2D,
		.extent    = { m_pImpl->swapchainExtent.width, m_pImpl->swapchainExtent.height, 1 },
		.format    = std::make_pair(gpu::EImageFormatComponents::Depth, gpu::EImageFormatType::F32),
		.usageBits = gpu::EImageUsageBits::TransferSrc | gpu::EImageUsageBits::TransferDst | gpu::EImageUsageBits::Storage | gpu::EImageUsageBits::DepthStencilAttachment
	});

	m_pImpl->mainCommandPool    = std::make_unique<gpu::CommandPool>(vME);
	m_pImpl->mainDescriptorPool = std::make_unique<gpu::DescriptorPool>(vME);

	auto cmdPoolInfo         = gpu_detail::command_pool_create_info(m_pImpl->graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	auto fenceCreateInfo     = gpu_detail::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	auto semaphoreCreateInfo = gpu_detail::semaphore_create_info();

	for (auto& frameData : m_pImpl->perFrameData)
	{
		frameData.commandBuffer = &m_pImpl->mainCommandPool->Acquire();
		vkCreateSemaphore(m_pImpl->device, &semaphoreCreateInfo, nullptr, &frameData.swapchainSemaphore);
	}

	m_pImpl->NewFrame();
}

GraphicsContext::~GraphicsContext() noexcept
{
	WaitForIdle();

	for (auto& frameData : m_pImpl->perFrameData)
	{
		vkDestroySemaphore(m_pImpl->device, frameData.swapchainSemaphore, nullptr);
		m_pImpl->mainCommandPool->Release(*frameData.commandBuffer);
	}

	m_pImpl->mainDescriptorPool.reset();
	m_pImpl->mainCommandPool.reset();
	m_pImpl->backbufferDepth.reset();
	m_pImpl->backbuffer.reset();

	vmaDestroyAllocator(m_pImpl->allocator);

	vkb::destroy_swapchain(m_pImpl->swapchain);
	for (auto image : m_pImpl->swapchainImages)
		delete image;
	m_pImpl->swapchainImages.clear();

	vkb::destroy_device(m_pImpl->device);
}

GraphicsContext::Impl& GraphicsContext::GetImpl()
{
	return *m_pImpl;
}

gpu::Image& GraphicsContext::Backbuffer()
{
	return *m_pImpl->backbuffer;
}

gpu::Image& GraphicsContext::BackbufferDepth()
{
	return *m_pImpl->backbufferDepth;
}

gpu::CommandPool& GraphicsContext::CommandPool()
{
	return *m_pImpl->mainCommandPool;
}

gpu::DescriptorPool& GraphicsContext::DescriptorPool()
{
	return *m_pImpl->mainDescriptorPool;
}

std::unique_ptr<gpu::Buffer> GraphicsContext::CreateBuffer(const gpu::Buffer::Info& info)
{
	return std::make_unique<gpu::Buffer>(vME, info);
}

std::unique_ptr<gpu::Buffer> GraphicsContext::CreateBuffer(const gpu::ShaderDataStore& store, gpu::EBufferUsageBits usage)
{
	return CreateBuffer(gpu::Buffer::Info { store.Size(), usage | gpu::EBufferUsageBits::CanUpload });
}

std::unique_ptr<gpu::Image> GraphicsContext::CreateImage(const gpu::Image::Info& info)
{
	return std::make_unique<gpu::Image>(vME, info);
}

std::unique_ptr<gpu::Shader> GraphicsContext::CreateShader(const gpu::Shader::Info& info)
{
	return std::make_unique<gpu::Shader>(vME, info);
}

std::unique_ptr<gpu::Pipeline> GraphicsContext::CreatePipeline(const gpu::Pipeline::GraphicsPipelineInfo& info)
{
	return std::make_unique<gpu::Pipeline>(vME, info);
}

std::unique_ptr<gpu::Pipeline> GraphicsContext::CreatePipeline(const gpu::Pipeline::GraphicsPipelineInfo2& info)
{
	return std::make_unique<gpu::Pipeline>(vME, info);
}

std::unique_ptr<gpu::Pipeline> GraphicsContext::CreatePipeline(const gpu::Pipeline::ComputePipelineInfo& info)
{
	return std::make_unique<gpu::Pipeline>(vME, info);
}

void GraphicsContext::Submit(gpu::CommandBuffer& cmdBuffer, const std::vector<gpu::CommandBuffer*>& dependencies /* = {} */)
{
	std::vector<std::pair<VkPipelineStageFlagBits2, VkSemaphore>> waitInfo;
	for (auto d : dependencies)
		waitInfo.push_back(std::make_pair(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, d->GetImpl().semaphore));

	m_pImpl->SubmitInternal(cmdBuffer.GetImpl().cmd, waitInfo, {{ VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, cmdBuffer.GetImpl().semaphore }}, cmdBuffer.GetImpl().fence);
	m_pImpl->lastSubmittedCommandBuffer = &cmdBuffer;
}

void GraphicsContext::WaitForIdle()
{
	vkDeviceWaitIdle(m_pImpl->device);
}

void GraphicsContext::Present()
{
	m_pImpl->GetCurrentFrame().commandBuffer->Reset();
	if (auto encoder = m_pImpl->GetCurrentFrame().commandBuffer->Record("Present", gpu::ECommandBufferRecordType::OneTimeSubmit); encoder)
	{
		auto backbufferExtent = Backbuffer().GetImpl().extent;

		encoder->CopyImage(
			Backbuffer(),
			*m_pImpl->swapchainImages[m_pImpl->swapchainImageIndex],
			glm::uvec3(backbufferExtent.width, backbufferExtent.height, 1),
			glm::uvec3(m_pImpl->swapchainExtent.width, m_pImpl->swapchainExtent.height, 1)
		);

		encoder->TransitionImage(*m_pImpl->swapchainImages[m_pImpl->swapchainImageIndex], gpu::EImageLayout::PresentSrc);
	}

	std::vector<std::pair<VkPipelineStageFlagBits2, VkSemaphore>> waitInfo;
	waitInfo.push_back(std::make_pair(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, m_pImpl->GetCurrentFrame().swapchainSemaphore));

	if (m_pImpl->lastSubmittedCommandBuffer)
		waitInfo.push_back(std::make_pair(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, m_pImpl->lastSubmittedCommandBuffer->GetImpl().semaphore));

	std::vector<std::pair<VkPipelineStageFlagBits2, VkSemaphore>> signalInfo;
	signalInfo.push_back(std::make_pair(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, m_pImpl->GetCurrentFrame().commandBuffer->GetImpl().semaphore));

	m_pImpl->SubmitInternal(
		m_pImpl->GetCurrentFrame().commandBuffer->GetImpl().cmd,
		waitInfo, signalInfo,
		m_pImpl->GetCurrentFrame().commandBuffer->GetImpl().fence
	);

	m_pImpl->lastSubmittedCommandBuffer = nullptr;

	auto presentInfo = gpu_detail::present_info(m_pImpl->swapchain.swapchain, m_pImpl->GetCurrentFrame().commandBuffer->GetImpl().semaphore, m_pImpl->swapchainImageIndex);
	vkQueuePresentKHR(m_pImpl->graphicsQueue, &presentInfo);
	m_pImpl->swapchainImages[m_pImpl->swapchainImageIndex]->GetImpl().currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	++m_pImpl->currentFrame;
	m_pImpl->NewFrame();
}
