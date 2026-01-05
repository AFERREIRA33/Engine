#ifndef VDE__CORE__GRAPHICSCONTEXT_IMPL_VULKAN_H
#define VDE__CORE__GRAPHICSCONTEXT_IMPL_VULKAN_H
#pragma once

#include <core/graphicscontext.h>
#include <core/gpu/commandpool.h>
#include <core/gpu/descriptorpool.h>

#include <array>
#include <algorithm>
#include <vector>

#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

//efine ENABLE_VMA_TRACES

namespace vde::core::gpu
{
	class CommandBuffer;
}

struct vde::core::GraphicsContext::Impl
{
	static constexpr uint32_t kFRAME_OVERLAP = 2;

	vkb::Device                          device;
	VkQueue                              graphicsQueue;
	uint32_t                             graphicsQueueFamily;

	VmaAllocator                         allocator;

	vkb::Swapchain                       swapchain;
	VkFormat                             swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkExtent2D                           swapchainExtent;
	std::vector<gpu::Image*>             swapchainImages;
	uint32_t                             swapchainImageIndex = 0;
	std::unique_ptr<gpu::Image>          backbuffer = nullptr;
	std::unique_ptr<gpu::Image>          backbufferDepth = nullptr;

	std::unique_ptr<gpu::CommandPool>    mainCommandPool            = nullptr;
	gpu::CommandBuffer*                  lastSubmittedCommandBuffer = nullptr;

	std::unique_ptr<gpu::DescriptorPool> mainDescriptorPool = nullptr;

	struct PerFrameData
	{
		gpu::CommandBuffer* commandBuffer      = nullptr;
		VkSemaphore         swapchainSemaphore = VK_NULL_HANDLE;
	};
	
	std::array<PerFrameData, kFRAME_OVERLAP> perFrameData;

	size_t currentFrame = 0;

	auto& GetCurrentFrame();
	void  NewFrame();

	void  SubmitInternal(
		VkCommandBuffer cmdBuffer,
		const std::vector<std::pair<VkPipelineStageFlagBits2, VkSemaphore>>& waitInfo,
		const std::vector<std::pair<VkPipelineStageFlagBits2, VkSemaphore>>& signalInfo,
		VkFence fence = VK_NULL_HANDLE);
};

#ifdef ENABLE_VMA_TRACES
# ifndef NDEBUG
#  include <iostream>
#  define TRACE_VMA_ALLOC(D) \
	std::cout << "VMA ALLOC:" << __FILE__ << ":" << __LINE__ << " (" << __FUNCSIG__ << ")" << std::endl;
#  define TRACE_VMA_FREE(D) \
	std::cout << "VMA FREE:" << __FILE__ << ":" << __LINE__ << " (" << __FUNCSIG__ << ")" << std::endl;
# else
#  define TRACE_VMA_ALLOC(D)
#  define TRACE_VMA_FREE(D)
# endif
#else
# define TRACE_VMA_ALLOC(D) {}
# define TRACE_VMA_FREE(D)	{}
#endif /* ENABLE_VMA_TRACES */

#endif /* VDE__CORE__GRAPHICSCONTEXT_IMPL_VULKAN_H */
