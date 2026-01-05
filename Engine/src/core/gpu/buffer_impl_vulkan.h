#ifndef VDE__CORE__GPU__BUFFER_IMPL_VULKAN_H
#define VDE__CORE__GPU__BUFFER_IMPL_VULKAN_H
#pragma once

#include <core/gpu/buffer.h>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

struct vde::core::gpu::Buffer::Impl
{
	VkBuffer          buffer     = VK_NULL_HANDLE;
	VmaAllocation     allocation = VK_NULL_HANDLE;
	VmaAllocationInfo allocationInfo;

	size_t            size;
	EBufferUsageBits  usage;
};

#endif /* VDE__CORE__GPU__BUFFER_IMPL_VULKAN_H */
