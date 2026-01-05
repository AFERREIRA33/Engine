#include <core/assets/assetsource.h>
#include <fstream>

using namespace vde::core::assets;

FileAssetSource::FileAssetSource(const std::filesystem::path& path)
	: m_path(path)
{
	if (std::filesystem::exists(m_path))
		m_lastWriteTime = std::filesystem::last_write_time(m_path);
}

std::string FileAssetSource::Id() const
{
	return m_path.string();
}

bool FileAssetSource::HasChanged() const
{
	if (!std::filesystem::exists(m_path))
		return false;

	auto currentTime = std::filesystem::last_write_time(m_path);
	bool changed = currentTime != m_lastWriteTime;
	m_lastWriteTime = currentTime;
	return changed;
}

std::vector<std::byte> FileAssetSource::Data() const
{
	std::ifstream ifs(m_path, std::ios::binary | std::ios::ate);
	if (!ifs)
		throw std::runtime_error("Failed to open file: " + m_path.string());

	std::streamsize size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	std::vector<std::byte> result(size);
	if (!ifs.read(reinterpret_cast<char*>(result.data()), size))
		throw std::runtime_error("Failed to read file: " + m_path.string());

	return result;
}
