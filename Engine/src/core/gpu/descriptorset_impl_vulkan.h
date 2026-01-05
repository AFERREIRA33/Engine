#ifndef VDE__CORE__GPU__DESCRIPTORSET_IMPL_VULKAN_H
#define VDE__CORE__GPU__DESCRIPTORSET_IMPL_VULKAN_H
#pragma once

#include <core/gpu/descriptorset.h>
#include <VkBootstrap.h>

struct vde::core::gpu::DescriptorSetLayout::Impl
{
	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
};

struct vde::core::gpu::DescriptorSet::Impl
{
	VkDescriptorSet descriptorSet;
};

#endif /* VDE__CORE__GPU__DESCRIPTORSET_IMPL_VULKAN_H */
