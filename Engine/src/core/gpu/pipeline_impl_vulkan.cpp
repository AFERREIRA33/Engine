#include "pipeline_impl_vulkan.h"
#include "shader_impl_vulkan.h"
#include "image_impl_vulkan.h"
#include "../graphicscontext_impl_vulkan.h"
#include "../gpu_detail/vk_initializers.h"
#include "../gpu_detail/vk_converters.h"

using namespace vde::core::gpu;

void Pipeline::m_CreatePipelineFromInfo2(const GraphicsPipelineInfo2& info)
{	
	std::vector<VkPipelineShaderStageCreateInfo> stageInfos;
	for (auto& s : info.stages)
		stageInfos.push_back(gpu_detail::pipeline_shader_stage_create_info(s->GetImpl().stage, s->GetImpl().shaderModule, s->GetImpl().entryPoint.c_str()));

	VkGraphicsPipelineCreateInfo pipelineCreateInfo { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineCreateInfo.stageCount = uint32_t(stageInfos.size());
	pipelineCreateInfo.pStages    = stageInfos.data();

	VkPipelineViewportStateCreateInfo viewportState { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewportState.viewportCount = 1;
	viewportState.scissorCount  = 1;
	pipelineCreateInfo.pViewportState = &viewportState;

	VkPipelineVertexInputStateCreateInfo vertexInput { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };	
	std::vector<VkVertexInputAttributeDescription> attributes;
	std::vector<VkVertexInputBindingDescription>   bindings;

	Shader* vertexShader = nullptr;
	for (auto& s : info.stages)
	{
		if (s->GetImpl().stage & VK_SHADER_STAGE_VERTEX_BIT)
		{
			vertexShader = s;
			break;
		}
	}

	if (vertexShader)
	{
		auto vi = vertexShader->GetVertexInput();

		for (auto& a : vi.attributes)
		{
			attributes.push_back(a);

			bindings.push_back({
				.binding   = a.binding,
				.stride    = s_ToVkStride(a.format),
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
			});
		}
	}

	vertexInput.vertexAttributeDescriptionCount = uint32_t(attributes.size());
	vertexInput.pVertexAttributeDescriptions    = attributes.data();
	vertexInput.vertexBindingDescriptionCount   = uint32_t(bindings.size());
	vertexInput.pVertexBindingDescriptions      = bindings.data();

	pipelineCreateInfo.pVertexInputState = &vertexInput;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;

	VkPipelineRasterizationStateCreateInfo rasterization { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterization.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization.lineWidth   = 1.0f;
	pipelineCreateInfo.pRasterizationState = &rasterization;

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	for (size_t i = 0; i < info.colorAttachmentFormats.size(); ++i)
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachment {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachments.push_back(colorBlendAttachment);
	}

	VkPipelineColorBlendStateCreateInfo colorBlend { .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlend.logicOpEnable            = VK_FALSE;
	colorBlend.logicOp                  = VK_LOGIC_OP_COPY;
	colorBlend.attachmentCount          = uint32_t(info.colorAttachmentFormats.size());
	colorBlend.pAttachments             = colorBlendAttachments.data();
	pipelineCreateInfo.pColorBlendState = &colorBlend;

	VkPipelineMultisampleStateCreateInfo multisample { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample.minSampleShading = 1.0f;
	pipelineCreateInfo.pMultisampleState = &multisample;

	VkPipelineDepthStencilStateCreateInfo depthStencil { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	depthStencil.maxDepthBounds           = 1.f;
	depthStencil.depthTestEnable          = (info.depthAttachmentFormat.first != EImageFormatComponents::Undefined) ? VK_TRUE : VK_FALSE;
	depthStencil.depthWriteEnable         = (info.depthAttachmentFormat.first != EImageFormatComponents::Undefined) ? VK_TRUE : VK_FALSE;
	depthStencil.depthBoundsTestEnable    = VK_FALSE;
	depthStencil.depthCompareOp           = VK_COMPARE_OP_LESS;
	pipelineCreateInfo.pDepthStencilState = &depthStencil;

	VkPipelineLayoutCreateInfo layoutCreateInfo = gpu_detail::pipeline_layout_create_info();

	for (auto& s : info.stages)
	{
		for (auto i : s->GetDescriptorSetIndices())
			m_pImpl->descriptorSetLayouts.push_back(s->GetDescriptorSetLayout(i));
		
		s->GetImpl().descriptorSetLayouts.clear();
	}

	std::vector<VkDescriptorSetLayout> vkLayouts;
	for (auto& l : m_pImpl->descriptorSetLayouts)
		vkLayouts.push_back(l.GetImpl().layout);

	layoutCreateInfo.setLayoutCount = uint32_t(vkLayouts.size());
	layoutCreateInfo.pSetLayouts    = vkLayouts.data();

	std::vector<VkPushConstantRange> pushConstantRanges;
	for (auto& s : info.stages)
	{
		if (s->GetImpl().pushConstants.isValid)
		{
			pushConstantRanges.push_back(s->GetImpl().pushConstants.range);
			m_pImpl->pushConstantLayouts[s->Stage()] = {
				s->GetImpl().pushConstants.range,
				s->GetImpl().pushConstants.type
			};
		}
	}
	layoutCreateInfo.pushConstantRangeCount = uint32_t(pushConstantRanges.size());
	layoutCreateInfo.pPushConstantRanges    = pushConstantRanges.data();

	vkCreatePipelineLayout(m_owner->GetImpl().device, &layoutCreateInfo, nullptr, &m_pImpl->layout);
	pipelineCreateInfo.layout = m_pImpl->layout;

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicInfo { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicInfo.pDynamicStates       = dynamicStates;
	dynamicInfo.dynamicStateCount    = 2;
	pipelineCreateInfo.pDynamicState = &dynamicInfo;

	VkPipelineRenderingCreateInfo rendering { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	rendering.colorAttachmentCount  = uint32_t(info.colorAttachmentFormats.size());

	std::vector<VkFormat> colorAttachmentFormats;
	for (auto& f : info.colorAttachmentFormats)
		colorAttachmentFormats.push_back(s_ToVk(f));
	rendering.pColorAttachmentFormats = colorAttachmentFormats.data();

	if (info.depthAttachmentFormat.first != EImageFormatComponents::Undefined)
		rendering.depthAttachmentFormat = s_ToVk(info.depthAttachmentFormat);
	else
		rendering.depthAttachmentFormat = VK_FORMAT_UNDEFINED;

	pipelineCreateInfo.pNext = &rendering;

	vkCreateGraphicsPipelines(m_owner->GetImpl().device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pImpl->pipeline);
}

Pipeline::Pipeline(util::Badge<core::GraphicsContext> badge, const GraphicsPipelineInfo& desc)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
	, m_family(EPipelineFamily::Graphics)
{
	GraphicsPipelineInfo2 info {};
	info.stages = desc.stages;
	for (auto c : desc.colorAttachments)
		info.colorAttachmentFormats.push_back(c->Format());

	if (desc.depthAttachment)
		info.depthAttachmentFormat = desc.depthAttachment->Format();

	m_CreatePipelineFromInfo2(info);
}

Pipeline::Pipeline(util::Badge<core::GraphicsContext> badge, const GraphicsPipelineInfo2& info)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
	, m_family(EPipelineFamily::Graphics)
{
	m_CreatePipelineFromInfo2(info);
}

Pipeline::Pipeline(util::Badge<core::GraphicsContext> badge, const ComputePipelineInfo& info)
	: m_pImpl(new Impl)
	, m_owner(badge.Owner())
	, m_family(EPipelineFamily::Compute)
{
	VkPipelineLayoutCreateInfo layoutCreateInfo = gpu_detail::pipeline_layout_create_info();

	for (auto i : info.shader->GetDescriptorSetIndices())
		m_pImpl->descriptorSetLayouts.push_back(info.shader->GetDescriptorSetLayout(i));
	info.shader->GetImpl().descriptorSetLayouts.clear();

	std::vector<VkDescriptorSetLayout> vkLayouts;
	for (auto& l : m_pImpl->descriptorSetLayouts)
		vkLayouts.push_back(l.GetImpl().layout);

	layoutCreateInfo.setLayoutCount = uint32_t(vkLayouts.size());
	layoutCreateInfo.pSetLayouts = vkLayouts.data();

	std::vector<VkPushConstantRange> pushConstantRanges;
	if (info.shader->GetImpl().pushConstants.isValid)
	{
		pushConstantRanges.push_back(info.shader->GetImpl().pushConstants.range);
		m_pImpl->pushConstantLayouts[info.shader->Stage()] = {
			info.shader->GetImpl().pushConstants.range,
			info.shader->GetImpl().pushConstants.type
		};
	}
	layoutCreateInfo.pushConstantRangeCount = uint32_t(pushConstantRanges.size());
	layoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

	vkCreatePipelineLayout(m_owner->GetImpl().device, &layoutCreateInfo, nullptr, &m_pImpl->layout);

	VkComputePipelineCreateInfo pipelineCreateInfo { .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	pipelineCreateInfo.stage  = gpu_detail::pipeline_shader_stage_create_info(info.shader->GetImpl().stage, info.shader->GetImpl().shaderModule, info.shader->GetImpl().entryPoint.c_str());
	pipelineCreateInfo.layout = m_pImpl->layout;

	vkCreateComputePipelines(m_owner->GetImpl().device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pImpl->pipeline);
}

Pipeline::~Pipeline() noexcept
{
	for (auto& l : m_pImpl->descriptorSetLayouts)
		vkDestroyDescriptorSetLayout(m_owner->GetImpl().device, l.GetImpl().layout, nullptr);

	vkDestroyPipelineLayout(m_owner->GetImpl().device, m_pImpl->layout, nullptr);
	vkDestroyPipeline(m_owner->GetImpl().device, m_pImpl->pipeline, nullptr);
}

Pipeline::Impl& Pipeline::GetImpl()
{
	return *m_pImpl;
}

EPipelineFamily Pipeline::Family() const
{
	return m_family;
}


std::vector<uint32_t> Pipeline::GetDescriptorSetIndices()
{
	std::vector<uint32_t> result;

	for (auto& s : m_pImpl->descriptorSetLayouts)
		result.push_back(s.setId);

	return result;
}

DescriptorSetLayout& Pipeline::GetDescriptorSetLayout(size_t set)
{
	std::vector<DescriptorSetLayout>::iterator setIt = std::find_if(m_pImpl->descriptorSetLayouts.begin(), m_pImpl->descriptorSetLayouts.end(), [set](DescriptorSetLayout& l) { return l.setId == set; });
	if (setIt == m_pImpl->descriptorSetLayouts.end())
		throw std::runtime_error("Invalid descriptor set index");

	return *setIt;
}

std::optional<std::reference_wrapper<ShaderDataType>> Pipeline::GetPushConstantLayoutFor(EShaderStage stage)
{
	if (m_pImpl->pushConstantLayouts.contains(stage))
		return m_pImpl->pushConstantLayouts[stage].type;
	return std::nullopt;
}
