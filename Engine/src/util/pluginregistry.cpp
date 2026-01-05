#include <util/pluginregistry.h>

#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>

using namespace vde::util;

struct PluginRegistry::Impl
{
	struct Module
	{
		HMODULE     hModule;
		PluginBase* plugin;
	};

	std::unordered_map<EPluginFamily, std::vector<Module>> modules;
};

PluginRegistry::PluginRegistry()
	: m_pImpl(new Impl)
{
}

PluginRegistry::~PluginRegistry() noexcept
{
	for (auto& [_, plugins] : m_pImpl->modules)
	{
		for (auto& p : plugins)
		{
			delete p.plugin;
			FreeLibrary(p.hModule);
		}
	}
}

PluginContext& PluginRegistry::Context()
{
	return m_context;
}

void PluginRegistry::Load(const std::filesystem::path& path)
{
	HMODULE module = LoadLibraryW(path.c_str());
	if (module)
	{
		CreatePluginFn createFn = reinterpret_cast<CreatePluginFn>(GetProcAddress(module, "CreatePlugin"));
		if (createFn)
		{
			PluginBase* plugin = createFn(&m_context);
			m_pImpl->modules[plugin->Family()].push_back({ module, plugin });
		}
	}
}

void PluginRegistry::LoadAllFromDirectory(const std::filesystem::path& directory)
{
	if (!std::filesystem::is_directory(directory))
		return;

	for (const auto& e : std::filesystem::directory_iterator(directory))
		if (e.is_regular_file() && e.path().extension() == ".dll")
			Load(e.path());
}

std::vector<PluginBase*> PluginRegistry::ListFamily(EPluginFamily family) const
{
	if (!m_pImpl->modules.contains(family))
		return {};

	std::vector<PluginBase*> result;

	for (auto& m : m_pImpl->modules[family])
		result.push_back(m.plugin);

	return result;
}
