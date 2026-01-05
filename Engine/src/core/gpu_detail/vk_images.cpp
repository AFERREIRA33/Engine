#include "vk_images.h"
#include "vk_initializers.h"

void vde::core::gpu_detail::transition(VkCommandBuffer cmd, VkImage image, VkImageLayout from, VkImageLayout to)
{
    if (from == to)
        return;

    VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };

    imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = from;
    imageBarrier.newLayout = to;

    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBarrier.subresourceRange = image_subresource_range(aspectMask);
    imageBarrier.image            = image;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers    = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}

void vde::core::gpu_detail::transition_depth(VkCommandBuffer cmd, VkImage image, VkImageLayout from, VkImageLayout to)
{
    if (from == to)
        return;

    VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };

    imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

    imageBarrier.oldLayout = from;
    imageBarrier.newLayout = to;

    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    imageBarrier.subresourceRange = image_subresource_range(aspectMask);
    imageBarrier.image            = image;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;

    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers    = &imageBarrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}
