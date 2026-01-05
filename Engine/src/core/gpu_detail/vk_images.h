#ifndef VDE__CORE__GPU_DETAIL__VK_IMAGES_H
#define VDE__CORE__GPU_DETAIL__VK_IMAGES_H
#pragma once

#include "vk_types.h"

namespace vde::core::gpu_detail
{
	void transition(VkCommandBuffer cmd, VkImage image, VkImageLayout from, VkImageLayout to);
	void transition_depth(VkCommandBuffer cmd, VkImage image, VkImageLayout from, VkImageLayout to);
}

#endif /* VDE__CORE__GPU_DETAIL__VK_IMAGES_H */
