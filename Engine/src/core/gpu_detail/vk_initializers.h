#ifndef VDE__CORE__GPU_DETAIL__VK_INITIALIZERS_H
#define VDE__CORE__GPU_DETAIL__VK_INITIALIZERS_H
#pragma once

#include <vector>
#include "vk_types.h"

#include <core/gpu/enums.h>

namespace vde::core
{
    class GraphicsContext;
}

namespace vde::core::gpu_detail
{
    VkCommandPoolCreateInfo         command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    VkCommandBufferAllocateInfo     command_buffer_allocate_info(VkCommandPool pool, bool isSecondary = false, uint32_t count = 1);
    VkCommandBufferBeginInfo        command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);
    VkCommandBufferSubmitInfo       command_buffer_submit_info(VkCommandBuffer cmd);
    VkFenceCreateInfo               fence_create_info(VkFenceCreateFlags flags = 0);
    VkSemaphoreCreateInfo           semaphore_create_info(VkSemaphoreCreateFlags flags = 0);
    VkSubmitInfo2                   submit_info(VkCommandBufferSubmitInfo* cmd, const std::vector<VkSemaphoreSubmitInfo>& signalSemaphoreInfos = {}, const std::vector<VkSemaphoreSubmitInfo>& waitSemaphoreInfos = {});
    VkPresentInfoKHR                present_info(const VkSwapchainKHR& swapchain, const VkSemaphore& waitSemaphore, const uint32_t& imageIndex);
    VkRenderingAttachmentInfo       attachment_info(VkImageView view, VkClearValue* clear, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);
    VkRenderingAttachmentInfo       depth_attachment_info(VkImageView view, VkImageLayout layout /*= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL*/);
    VkRenderingInfo                 rendering_info(VkExtent2D renderExtent, const std::vector<VkRenderingAttachmentInfo>& colorAttachments, VkRenderingAttachmentInfo* depthAttachment);
    VkImageSubresourceRange         image_subresource_range(VkImageAspectFlags aspectMask);
    VkSemaphoreSubmitInfo           semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
    VkShaderModuleCreateInfo        shader_module_create_info(const void* data, size_t size);
    VkDescriptorSetLayoutBinding    descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding);
    VkDescriptorSetLayoutCreateInfo descriptorset_layout_create_info(VkDescriptorSetLayoutBinding* bindings, uint32_t bindingCount);
    VkWriteDescriptorSet            write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding);
    VkWriteDescriptorSet            write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding);
    VkDescriptorBufferInfo          buffer_info(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);
    VkImageCreateInfo               image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent, vde::core::gpu::EImageType imageType, uint32_t mipLevels = 1, uint32_t arrayLayers = 1);
    VkImageViewCreateInfo           imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags, vde::core::gpu::EImageType imageType, uint32_t mipLevels = 1, uint32_t arrayLayers = 1);
    VkPipelineLayoutCreateInfo      pipeline_layout_create_info();
    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char* entry = nullptr);

    class VkGraphicsPipelineBuilder
    {
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
    public:
        VkGraphicsPipelineBuilder& UseShaderStage(const VkPipelineShaderStageCreateInfo& stage);
        VkPipeline                 Build(core::GraphicsContext& graphicsContext);
    };
}

#endif /* VDE__CORE__GPU_DETAIL__VK_INITIALIZERS_H */