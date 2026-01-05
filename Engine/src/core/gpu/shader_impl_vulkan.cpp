#include "shader_impl_vulkan.h"
#include "../graphicscontext_impl_vulkan.h"
#include "../gpu_detail/vk_initializers.h"

#include "shader_impl_vulkan__reflect.h"

#include <fstream>

using namespace vde::core::gpu;

Shader::Shader(util::Badge<core::GraphicsContext> badge, const Info& info)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
{
	auto shader_module_create_info = gpu_detail::shader_module_create_info(info.data, info.size);
	vkCreateShaderModule(m_owner->GetImpl().device, &shader_module_create_info, nullptr, &m_pImpl->shaderModule);

	SpvReflectShaderModule reflectModule;
	spvReflectCreateShaderModule(info.size, info.data, &reflectModule);

	m_pImpl->entryPoint           = reflectModule.entry_point_name;
	m_pImpl->stage                = VkShaderStageFlagBits(reflectModule.shader_stage);
	m_pImpl->descriptorSetLayouts = ReflectDescriptorSetLayouts(m_owner->GetImpl().device, &reflectModule);

	if (reflectModule.shader_stage & SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
	{
		m_pImpl->vertexInput.isValid    = true;
		m_pImpl->vertexInput.attributes = ReflectVertexInput(&reflectModule);
	}

	m_pImpl->pushConstants = ReflectPushConstants(&reflectModule);

	spvReflectDestroyShaderModule(&reflectModule);
}

Shader::~Shader()
{
	for (auto& s : m_pImpl->descriptorSetLayouts)
		vkDestroyDescriptorSetLayout(m_owner->GetImpl().device, s.GetImpl().layout, nullptr);

	vkDestroyShaderModule(m_owner->GetImpl().device, m_pImpl->shaderModule, nullptr);
}

EShaderStage Shader::Stage() const
{
	switch (m_pImpl->stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:                  return EShaderStage::Vertex;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:    return EShaderStage::TessControl;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return EShaderStage::TessEval;
	case VK_SHADER_STAGE_GEOMETRY_BIT:                return EShaderStage::Geometry;
	case VK_SHADER_STAGE_FRAGMENT_BIT:                return EShaderStage::Fragment;
	case VK_SHADER_STAGE_COMPUTE_BIT:                 return EShaderStage::Compute;
	}

	return EShaderStage(-1);
}

Shader::Impl& Shader::GetImpl()
{
	return *m_pImpl;
}

Shader::VertexInput& Shader::GetVertexInput()
{
	return m_pImpl->vertexInput;
}

const ShaderDataType& Shader::PushConstants()
{
	return m_pImpl->pushConstants.type;
}

std::vector<uint32_t> Shader::GetDescriptorSetIndices()
{
	std::vector<uint32_t> result;

	for (auto& s : m_pImpl->descriptorSetLayouts)
		result.push_back(s.setId);

	return result;
}

DescriptorSetLayout& Shader::GetDescriptorSetLayout(size_t set)
{
	std::vector<DescriptorSetLayout>::iterator setIt = std::find_if(m_pImpl->descriptorSetLayouts.begin(), m_pImpl->descriptorSetLayouts.end(), [set](DescriptorSetLayout& l) { return l.setId == set; });
	if (setIt == m_pImpl->descriptorSetLayouts.end())
		throw std::runtime_error("Invalid descriptor set index");

	return *setIt;
}
