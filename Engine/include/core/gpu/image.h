#ifndef VDE__CORE__GPU__IMAGE_H
#define VDE__CORE__GPU__IMAGE_H
#pragma once

#include <memory>
#include <glm/glm.hpp>

#include <util/badge.h>
#include <util/property.h>
#include <core/gpu/enums.h>

namespace vde::core
{
	class GraphicsContext;
}

namespace vde::core::gpu
{
	struct PreexistingImageInfo;

	class Image
	{
		core::GraphicsContext* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

	public:
		struct Info
		{
			EImageType      type;
			glm::uvec3      extent;
			ImageFormat     format;
			EImageUsageBits usageBits;
			uint32_t        mipCount    = 1;
			uint32_t        layerCount  = 1;
			const void*     initialData = nullptr;
		};

		explicit Image();
		explicit Image(util::Badge<core::GraphicsContext> badge, const Info& info);
		explicit Image(util::Badge<core::GraphicsContext> badge, const PreexistingImageInfo& info);
		~Image() noexcept;

		Impl& GetImpl();

		glm::uvec3  Size() const;
		size_t      ArraySize() const;
		ImageFormat Format() const;

		void Upload(const glm::vec3& offset, const glm::uvec3& size, const void* pixels);
	};
}

VDE_UTIL_DECLARE_PROPERTY_TYPE_NONINTERPOLABLE(vde::core::gpu::Image*);

#endif /* VDE__CORE__GPU__IMAGE_H */
