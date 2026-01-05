#ifndef VDE__CORE__GPU__COMMANDBUFFER_H
#define VDE__CORE__GPU__COMMANDBUFFER_H
#pragma once


#include <memory>
#include <vector>
#include <core/gpu/enums.h>
#include <core/gpu/shader.h>
#include <core/gpu/image.h>
#include <util/badge.h>

#include <glm/glm.hpp>

namespace vde::core::gpu
{
	class Buffer;
	class Image;
	class Pipeline;
	class CommandBuffer;
	class CommandPool;
	class CommandEncoder;
	class DescriptorSet;

	enum class ECommandBufferRecordType
	{
		Normal,
		OneTimeSubmit
	};

	struct IndirectDrawCommand
	{
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t  vertexOffset;
		uint32_t firstInstance;
	};

	class m_PipelineBasedCommandEncoder
	{
	protected:
		CommandBuffer* m_owner;
		Pipeline* m_pipeline;

		explicit m_PipelineBasedCommandEncoder(CommandBuffer* owner);
		explicit m_PipelineBasedCommandEncoder(CommandBuffer* owner, Pipeline& pipeline);

	public:
		explicit m_PipelineBasedCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner);
		explicit m_PipelineBasedCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, Pipeline& pipeline);
		~m_PipelineBasedCommandEncoder() noexcept;

		void UpdatePushConstants(EShaderStage stage, const ShaderDataStore& pushConstants);
		void BindDescriptorSets(size_t firstSet, const std::vector<DescriptorSet*>& sets);
	};

	class RenderingCommandEncoder
		: public m_PipelineBasedCommandEncoder
	{
		bool m_shouldEndRendering = true;

	public:
		explicit RenderingCommandEncoder(CommandBuffer* owner, Pipeline& pipeline);
		explicit RenderingCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, const std::vector<Image*>& colorAttachments, Image* depthAttachment);
		explicit RenderingCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, Pipeline& pipeline, const std::vector<Image*>& colorAttachments, Image* depthAttachment);
		~RenderingCommandEncoder() noexcept;

		void SetViewport(const glm::vec2& origin, const glm::vec2& size, const glm::vec2& depthBounds = { 0.0f, 1.0f }, bool setScissorToo = true);

		void BindPipeline(Pipeline& pipeline);
		void Draw(const std::vector<Buffer*>& vertexBuffers, uint32_t numVertices, uint32_t numInstances = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
		void DrawIndexed(const std::vector<Buffer*>& vertexBuffers, Buffer* indexBuffer, uint32_t numIndices, uint32_t numInstances = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0);
		void DrawIndexedIndirect(const std::vector<Buffer*>& vertexBuffers, Buffer* indexBuffer, Buffer* indirectBuffer, size_t drawCount, size_t offset, size_t stride = sizeof(IndirectDrawCommand));
		void DrawIndexedSubmeshes(const std::vector<Buffer*>& vertexBuffers, Buffer* indexBuffer, const std::vector<std::pair<uint32_t, uint32_t>>& submeshes, uint32_t numInstances = 1);
	};

	class ComputeCommandEncoder
		: public m_PipelineBasedCommandEncoder
	{
	public:
		explicit ComputeCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner);
		explicit ComputeCommandEncoder(util::Badge<CommandEncoder> badge, CommandBuffer* owner, Pipeline& pipeline);
		~ComputeCommandEncoder() noexcept;

		void Dispatch(const glm::uvec3& groupCount);
	};

	class CommandEncoder
	{
		CommandBuffer* m_owner;

	public:
		explicit CommandEncoder(util::Badge<CommandBuffer> badge, const std::string& name, ECommandBufferRecordType recordType = ECommandBufferRecordType::Normal, const std::vector<ImageFormat>& colorFormats = {}, const ImageFormat& depthFormat = {});
		~CommandEncoder();

		void TransitionImage(Image& image, EImageLayout newLayout);
		void CopyImage(Image& src, Image& dst, const glm::uvec3& srcSize, const glm::uvec3& dstSize);
		void ClearImageColor(Image& image, const glm::vec4& color);
		void ClearImageDepthStencil(Image& image, float depth, uint32_t stencil = 0x0);

		std::unique_ptr<RenderingCommandEncoder> BeginRendering(const std::vector<Image*>& colorAttachments, Image* depthAttachment); // Special case for external calls to Vk (ie. imgui)
		std::unique_ptr<RenderingCommandEncoder> BeginRendering(Pipeline& pipeline, const std::vector<Image*>& colorAttachments, Image* depthAttachment);

		std::unique_ptr<ComputeCommandEncoder> BeginCompute(); // Special case for external calls to Vk
		std::unique_ptr<ComputeCommandEncoder> BeginCompute(Pipeline& pipeline);

		void ExecuteCommands(CommandBuffer& secondary);
	};

	class CommandBuffer
	{
		CommandPool* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;
		
	public:
		explicit CommandBuffer(util::Badge<core::gpu::CommandPool> badge, bool isSecondary = false);
		~CommandBuffer() noexcept;

		Impl& GetImpl();
		CommandPool& Owner();

		bool IsSignaled() const;
		void WaitForSignal() const;
		void Reset();
		std::unique_ptr<CommandEncoder> Record(const std::string& name, ECommandBufferRecordType recordType = ECommandBufferRecordType::Normal, const std::vector<ImageFormat>& colorFormats = {}, const ImageFormat& depthFormat = {});
	};
}

#endif /* VDE__CORE__GPU__COMMANDBUFFER_H */
