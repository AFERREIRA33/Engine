#ifndef VDE__CORE__GPU__COMMANDPOOL_IMPL_VULKAN_H
#define VDE__CORE__GPU__COMMANDPOOL_IMPL_VULKAN_H
#pragma once

#include <core/gpu/commandpool.h>
#include <VkBootstrap.h>

struct vde::core::gpu::CommandPool::Impl
{
	VkCommandPool pool;
};

#endif /* VDE__CORE__GPU__COMMANDPOOL_IMPL_VULKAN_H */
