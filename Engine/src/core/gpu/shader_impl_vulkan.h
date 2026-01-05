#ifndef VDE__CORE__GPU__SHADER_IMPL_VULKAN_H
#define VDE__CORE__GPU__SHADER_IMPL_VULKAN_H
#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include <VkBootstrap.h>

#include <core/gpu/shader.h>
#include <core/gpu/image.h>

#include "descriptorset_impl_vulkan.h"

struct vde::core::gpu::Shader::VertexInput
{
	bool                                           isValid = false;
	std::vector<VkVertexInputAttributeDescription> attributes;
};

struct vde::core::gpu::Shader::PushConstantsLayout
{
	bool                isValid = false;
	VkPushConstantRange range = {};
	ShaderDataType      type;
};

struct vde::core::gpu::Shader::Impl
{
	VkShaderModule                   shaderModule = VK_NULL_HANDLE;

	std::string                      entryPoint;
	VkShaderStageFlagBits            stage;

	VertexInput                      vertexInput;
	PushConstantsLayout              pushConstants;
	std::vector<DescriptorSetLayout> descriptorSetLayouts;
};

#endif /* VDE__CORE__GPU__SHADER_IMPL_VULKAN_H */
