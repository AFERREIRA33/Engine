#ifndef VDE__CORE__ASSETS__ASSET_H
#define VDE__CORE__ASSETS__ASSET_H
#pragma once

#include <core/assets/assetsource.h>
#include <util/plugin.h>
#include <util/pluginregistry.h>
#include <util/property.h>

namespace vde::core::assets
{
	template<typename T>
	class Asset
	{
		std::unique_ptr<AssetSource> m_source;
		std::unique_ptr<T>           m_value;

	public:
		explicit Asset()
			: m_source(nullptr)
		{
		}

		template<typename... ARGS>
		explicit Asset(std::unique_ptr<AssetSource> src, ARGS... args)
			: m_source(std::move(src))
		{
			Load(args...);
		}

		Asset& operator=(std::unique_ptr<AssetSource>&& src)
		{
			m_source = std::move(src);
		}

		template<typename... ARGS>
		void Load(ARGS... args);

		template<typename... ARGS>
		bool ReloadIfChanged(ARGS... args)
		{
			if (m_source->HasChanged())
			{
				Load(args...);
				return true;
			}

			return false;
		}

		T& Value() { return *m_value; }
		operator T& () { return Value(); }
	};
}

#endif /* VDE__CORE__ASSETS__ASSET_H */
