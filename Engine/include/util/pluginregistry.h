#ifndef VDE__UTIL__PLUGINREGISTRY_H
#define VDE__UTIL__PLUGINREGISTRY_H
#pragma once

#include <util/plugin.h>
#include <util/globalinstance.h>

#include <filesystem>
#include <memory>

namespace vde::util
{
	class PluginRegistry
		: public GlobalInstance<PluginRegistry>
	{
		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

		PluginContext m_context;

	public:
		explicit PluginRegistry();
		~PluginRegistry() noexcept;

		PluginContext& Context();

		void Load(const std::filesystem::path& path);
		void LoadAllFromDirectory(const std::filesystem::path& directory);

		std::vector<PluginBase*> ListFamily(EPluginFamily family) const;

		template<typename T>
		std::vector<T*> List() const
		{
			auto list = ListFamily(T::kFAMILY);

			std::vector<T*> result;

			for (auto i : list)
				result.push_back(reinterpret_cast<T*>(i));

			return result;
		}
	};
}

#endif /* VDE__UTIL__PLUGINREGISTRY_H */
