#ifndef VDE__CORE__GPU_DETAIL__VK_CONVERTERS_H
#define VDE__CORE__GPU_DETAIL__VK_CONVERTERS_H
#pragma once

#include <tuple>
#include <core/gpu/image.h>
#include <vulkan/vulkan_core.h>

extern VkImageLayout               s_ToVk(vde::core::gpu::EImageLayout layout);
extern size_t                      s_ToVkPixelSize(VkFormat format);
extern VkFormat                    s_ToVk(const vde::core::gpu::ImageFormat& value);
extern vde::core::gpu::ImageFormat s_ToVde(VkFormat format);
extern VkImageUsageFlags           s_ToVk(vde::core::gpu::EImageUsageBits bits);
extern uint32_t                    s_ToVkStride(VkFormat format);

extern std::tuple<VkImageType, VkImageViewType, VkImageCreateFlags> s_ToVk(vde::core::gpu::EImageType type, uint32_t arrayLayers);

#endif /* VDE__CORE__GPU_DETAIL__VK_CONVERTERS_H */
