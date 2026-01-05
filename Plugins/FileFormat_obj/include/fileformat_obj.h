#ifndef VDEPLUGIN__FILEFORMAT_OBJ_H
#define VDEPLUGIN__FILEFORMAT_OBJ_H
#pragma once

#include <core/graphicscontext.h>
#include <graphics/mesh.h>
#include <util/plugin.h>

class __declspec(dllexport) FileFormat_OBJ
	: public vde::util::FileFormatPlugin<vde::graphics::Mesh>
{
	vde::core::GraphicsContext* m_graphicsContext;

public:
	explicit FileFormat_OBJ(vde::core::GraphicsContext* graphicsContext);

	bool CanLoad(const std::string& path) override;
	std::unique_ptr<vde::graphics::Mesh> Load(const void* data, size_t size) override;
};

#endif /* VDEPLUGIN__FILEFORMAT_OBJ_H */
