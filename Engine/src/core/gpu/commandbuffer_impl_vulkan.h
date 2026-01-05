#ifndef VDE__CORE__GPU__COMMANDBUFFER_IMPL_VULKAN_H
#define VDE__CORE__GPU__COMMANDBUFFER_IMPL_VULKAN_H
#pragma once

#include <core/gpu/commandbuffer.h>
#include <VkBootstrap.h>

struct vde::core::gpu::CommandBuffer::Impl
{
	VkCommandBuffer cmd;
	VkSemaphore     semaphore;
	VkFence         fence;
	bool            isSecondary;
};

#endif /* VDE__CORE__GPU__COMMANDBUFFER_IMPL_VULKAN_H */
