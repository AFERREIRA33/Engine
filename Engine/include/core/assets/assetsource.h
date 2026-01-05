#ifndef VDE__CORE__ASSETS__ASSETSOURCE_H
#define VDE__CORE__ASSETS__ASSETSOURCE_H
#pragma once

#include <filesystem>
#include <string>

namespace vde::core::assets
{
	class AssetSource
	{
	public:
		virtual ~AssetSource() noexcept = default;

		virtual std::string            Id()         const = 0;
		virtual bool                   HasChanged() const = 0;
		virtual std::vector<std::byte> Data()       const = 0;
	};

	class FileAssetSource
		: public AssetSource
	{
		std::filesystem::path                   m_path;
		mutable std::filesystem::file_time_type m_lastWriteTime;

	public:
		explicit FileAssetSource(const std::filesystem::path& path);

		std::string            Id()         const override;
		bool                   HasChanged() const override;
		std::vector<std::byte> Data()       const override;
	};
}

#endif /* VDE__CORE__ASSETS__ASSETSOURCE_H */
