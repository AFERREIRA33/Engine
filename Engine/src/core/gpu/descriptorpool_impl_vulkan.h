#ifndef VDE__CORE__GPU__DESCRIPTORPOOL_IMPL_VULKAN_H
#define VDE__CORE__GPU__DESCRIPTORPOOL_IMPL_VULKAN_H
#pragma once

#include <core/gpu/descriptorpool.h>
#include <VkBootstrap.h>

struct vde::core::gpu::DescriptorPool::Impl
{
	VkDescriptorPool pool = VK_NULL_HANDLE;
};

#endif /* VDE__CORE__GPU__DESCRIPTORPOOL_IMPL_VULKAN_H */
