#ifndef VDE__GRAPHICS__MESH_H
#define VDE__GRAPHICS__MESH_H
#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include <core/gpu/buffer.h>
#include <core/gpu/commandbuffer.h>

namespace vde::core::gpu
{
	class RenderingCommandEncoder;
}

namespace vde::graphics
{
	struct Mesh
	{
		struct Primitive
		{
			std::string name;
			uint32_t    startIndex;
			uint32_t    indexCount;
			bool        enabled = true;
		};

		static constexpr size_t kMAX_BUFFERS = 4; // For now : position, normal, UV, tangents

		std::array<std::unique_ptr<vde::core::gpu::Buffer>, kMAX_BUFFERS> vertexBuffers = { nullptr };
		std::unique_ptr<vde::core::gpu::Buffer>                           indexBuffer   = nullptr;
		std::vector<Primitive>                                            primitives    = {};

		void Draw(vde::core::gpu::RenderingCommandEncoder& encoder, uint32_t numInstances = 1) const;
		std::vector<vde::core::gpu::IndirectDrawCommand> DrawIndirect(vde::core::gpu::RenderingCommandEncoder& encoder, vde::core::gpu::Buffer& indirectBuffer, size_t offset) const;
	};
}

#endif /* VDE__GRAPHICS__MESH_H */
