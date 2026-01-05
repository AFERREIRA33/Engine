#ifndef VDEPLUGIN__FILEFORMAT_STBIMAGE_H
#define VDEPLUGIN__FILEFORMAT_STBIMAGE_H
#pragma once

#include <core/graphicscontext.h>
#include <core/gpu/image.h>
#include <util/plugin.h>

class __declspec(dllexport) FileFormat_stbimage
	: public vde::util::FileFormatPlugin<vde::core::gpu::Image>
{
	vde::core::GraphicsContext* m_graphicsContext;

public:
	explicit FileFormat_stbimage(vde::core::GraphicsContext* graphicsContext);

	bool CanLoad(const std::string& path) override;
	std::unique_ptr<vde::core::gpu::Image> Load(const void* data, size_t size) override;
};

#endif /* VDEPLUGIN__FILEFORMAT_STBIMAGE_H */
