#include <core/assets/asset.h>

#include <core/graphicscontext.h>
#include <core/gpu/shader.h>

using namespace vde::core::assets;

template<>
template<>
void Asset<vde::core::gpu::Shader>::Load<vde::core::GraphicsContext*>(vde::core::GraphicsContext* graphicsContext)
{
	auto data = m_source->Data();
	m_value = graphicsContext->CreateShader(vde::core::gpu::Shader::Info { data.data(), data.size() });
}
