#ifndef VDE__CORE__GPU__PIPELINE_IMPL_VULKAN_H
#define VDE__CORE__GPU__PIPELINE_IMPL_VULKAN_H
#pragma once

#include <unordered_map>
#include <vector>

#include <core/gpu/pipeline.h>
#include <VkBootstrap.h>

namespace vde::core::gpu
{
	struct PushConstantsLayout
	{
		VkPushConstantRange range = {};
		ShaderDataType      type;
	};
}

struct vde::core::gpu::Pipeline::Impl
{
	VkPipeline                                            pipeline = VK_NULL_HANDLE;
	VkPipelineLayout                                      layout   = VK_NULL_HANDLE;
	std::vector<DescriptorSetLayout>                      descriptorSetLayouts;
	std::unordered_map<EShaderStage, PushConstantsLayout> pushConstantLayouts;
};

#endif /* VDE__CORE__GPU__PIPELINE_IMPL_VULKAN_H */
