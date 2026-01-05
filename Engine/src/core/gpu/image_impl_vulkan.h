#ifndef VDE__CORE__GPU__IMAGE_IMPL_VULKAN_H
#define VDE__CORE__GPU__IMAGE_IMPL_VULKAN_H
#pragma once

#include <core/gpu/enums.h>
#include <core/gpu/image.h>

#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

struct vde::core::gpu::Image::Impl
{
	VmaAllocation allocation    = VK_NULL_HANDLE;

	VkImage         image         = VK_NULL_HANDLE;
	VkImageView     imageView     = VK_NULL_HANDLE;
	VkSampler       imageSampler  = VK_NULL_HANDLE;
	VkFormat        format        = VK_FORMAT_UNDEFINED;
	VkExtent3D      extent        = {};
	uint32_t        mipCount      = 1;
	uint32_t        layerCount    = 1;

	VkImageLayout   currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	EImageUsageBits usage = {};
};

struct vde::core::gpu::PreexistingImageInfo
{
	VkImage     image     = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;
	VkFormat    format    = VK_FORMAT_UNDEFINED;
	VkExtent3D  extent    = {};
	ImageFormat vdeFormat = {};
};

extern VkImageLayout     s_ToVk(vde::core::gpu::EImageLayout layout);
extern VkFormat          s_ToVk(const vde::core::gpu::ImageFormat& value);
extern VkImageUsageFlags s_ToVk(vde::core::gpu::EImageUsageBits bits);

#endif /* VDE__CORE__GPU__IMAGE_IMPL_VULKAN_H */
