#include <graphics/mesh.h>

#include <core/gpu/buffer.h>
#include <core/gpu/commandbuffer.h>

using namespace vde::graphics;

void Mesh::Draw(vde::core::gpu::RenderingCommandEncoder& encoder, uint32_t numInstances /* = 1 */) const
{
	std::vector<vde::core::gpu::Buffer*> buffers;
	for (auto& b : vertexBuffers)
		buffers.push_back(b.get());

	std::vector<std::pair<uint32_t, uint32_t>> submeshes;
	for (auto& p : primitives)
		if (p.enabled)
			submeshes.push_back({ p.startIndex, p.indexCount });

	encoder.DrawIndexedSubmeshes(buffers, indexBuffer.get(), submeshes, numInstances);
}

std::vector<vde::core::gpu::IndirectDrawCommand> Mesh::DrawIndirect(vde::core::gpu::RenderingCommandEncoder& encoder, vde::core::gpu::Buffer& indirectBuffer, size_t offset) const
{
	std::vector<vde::core::gpu::Buffer*> buffers;
	for (auto& b : vertexBuffers)
		buffers.push_back(b.get());

	std::vector<core::gpu::IndirectDrawCommand> result;

	size_t numMeshes = 0;
	for (auto& p : primitives)
	{
		if (p.enabled)
		{
			++numMeshes;
			result.push_back({ p.indexCount, 0, p.startIndex, 0, 0 });
		}
	}

	encoder.DrawIndexedIndirect(buffers, indexBuffer.get(), &indirectBuffer, numMeshes, offset);

	return result;
}
