#include <core/assets/asset.h>
#include <graphics/mesh.h>
#include <util/pluginregistry.h>

using namespace vde::core::assets;

template<>
template<>
void Asset<vde::graphics::Mesh>::Load<>()
{
	auto data = m_source->Data();

	for (auto plugin : vde::util::PluginRegistry::Global().List<vde::util::FileFormatPluginBase>())
	{
		if (plugin->CanLoad(m_source->Id()))
		{
			m_value = reinterpret_cast<vde::util::FileFormatPlugin<vde::graphics::Mesh>*>(plugin)->Load(data.data(), data.size());
			break;
		}
	}
}
