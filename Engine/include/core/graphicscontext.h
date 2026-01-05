#ifndef VDE__CORE__GRAPHICSCONTEXT_H
#define VDE__CORE__GRAPHICSCONTEXT_H
#pragma once

#include <functional>
#include <memory>

#include <core/gpu/buffer.h>
#include <core/gpu/image.h>
#include <core/gpu/shader.h>
#include <core/gpu/pipeline.h>

namespace vde::core
{
	class Window;

	namespace gpu
	{
		class CommandBuffer;
		class CommandPool;
		class DescriptorPool;
		class ShaderDataStore;
	}

	class GraphicsContext
	{
		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

	public:
		explicit GraphicsContext(Window& window);
		~GraphicsContext() noexcept;

		Impl&                GetImpl();
		gpu::Image&          Backbuffer();
		gpu::Image&          BackbufferDepth();
		gpu::CommandPool&    CommandPool();
		gpu::DescriptorPool& DescriptorPool();

		std::unique_ptr<gpu::Buffer>   CreateBuffer(const gpu::Buffer::Info& info);
		std::unique_ptr<gpu::Buffer>   CreateBuffer(const gpu::ShaderDataStore& store, gpu::EBufferUsageBits usage);
		std::unique_ptr<gpu::Image>    CreateImage(const gpu::Image::Info& info);
		std::unique_ptr<gpu::Shader>   CreateShader(const gpu::Shader::Info& info);
		std::unique_ptr<gpu::Pipeline> CreatePipeline(const gpu::Pipeline::GraphicsPipelineInfo& info);
		std::unique_ptr<gpu::Pipeline> CreatePipeline(const gpu::Pipeline::GraphicsPipelineInfo2& info);
		std::unique_ptr<gpu::Pipeline> CreatePipeline(const gpu::Pipeline::ComputePipelineInfo& info);

		void Submit(gpu::CommandBuffer& cmdBuffer, const std::vector<gpu::CommandBuffer*>& dependencies = {});

		void WaitForIdle();
		void Present();

		template<typename T>
		std::unique_ptr<gpu::Buffer> CreateBuffer(size_t count, gpu::EBufferUsageBits usage)
		{
			return CreateBuffer({ count * sizeof(T), usage });
		}

		template<typename T, size_t N>
		std::unique_ptr<gpu::Buffer> CreateBuffer(const std::array<T, N>& data, gpu::EBufferUsageBits usage)
		{
			auto result = CreateBuffer({ data.size() * sizeof(T), usage | gpu::EBufferUsageBits::CanUpload });
			result->Upload(data);
			return std::move(result);
		}
	};
}

#endif /* #define VDE__CORE__GRAPHICSCONTEXT_H */
