#ifndef VDE__CORE__GPU__SHADER_IMPL_VULKAN__REFLECT_H
#define VDE__CORE__GPU__SHADER_IMPL_VULKAN__REFLECT_H
#pragma once

#include <vector>
#include <VkBootstrap.h>
#include <SPIRV-Reflect/spirv_reflect.h>

#include "shader_impl_vulkan.h"

namespace vde::core::gpu
{
	std::vector<vde::core::gpu::DescriptorSetLayout> ReflectDescriptorSetLayouts(VkDevice device, SpvReflectShaderModule* module);
	std::vector<VkVertexInputAttributeDescription>   ReflectVertexInput(SpvReflectShaderModule* module);
	vde::core::gpu::Shader::PushConstantsLayout      ReflectPushConstants(SpvReflectShaderModule* module);
}

#endif /* VDE__CORE__GPU__SHADER_IMPL_VULKAN__REFLECT_H */
