#ifndef VDE__CORE__GPU__PIPELINE_H
#define VDE__CORE__GPU__PIPELINE_H
#pragma once

#include <core/gpu/enums.h>
#include <core/gpu/image.h>
#include <core/gpu/shader.h>
#include <util/badge.h>

#include <memory>
#include <vector>

namespace vde::core
{
	class GraphicsContext;
}

namespace vde::core::gpu
{
	class Shader;

	class Pipeline
	{
		core::GraphicsContext* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

		EPipelineFamily m_family;

	public:
		struct GraphicsPipelineInfo
		{
			std::vector<Shader*> stages;
			std::vector<Image*>  colorAttachments;
			Image*               depthAttachment;
		};

		struct GraphicsPipelineInfo2
		{
			std::vector<Shader*>     stages;
			std::vector<ImageFormat> colorAttachmentFormats;
			ImageFormat              depthAttachmentFormat;
		};

		struct ComputePipelineInfo
		{
			Shader* shader;
		};

		explicit Pipeline(util::Badge<core::GraphicsContext> badge, const GraphicsPipelineInfo& desc);
		explicit Pipeline(util::Badge<core::GraphicsContext> badge, const GraphicsPipelineInfo2& desc);
		explicit Pipeline(util::Badge<core::GraphicsContext> badge, const ComputePipelineInfo& desc);
		~Pipeline() noexcept;

		Impl& GetImpl();

		EPipelineFamily Family() const;

		std::vector<uint32_t> GetDescriptorSetIndices();
		DescriptorSetLayout&  GetDescriptorSetLayout(size_t set);
		std::optional<std::reference_wrapper<ShaderDataType>> GetPushConstantLayoutFor(EShaderStage stage);

	private:
		void m_CreatePipelineFromInfo2(const GraphicsPipelineInfo2& desc);
	};
}

#endif /* VDE__CORE__GPU__PIPELINE_H */
