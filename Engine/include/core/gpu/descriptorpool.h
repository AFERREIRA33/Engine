#ifndef VDE__CORE__GPU__DESCRIPTORPOOL_H
#define VDE__CORE__GPU__DESCRIPTORPOOL_H
#pragma once

#include <memory>
#include <util/badge.h>

#include <core/gpu/descriptorset.h>

namespace vde::core
{
	class GraphicsContext;
}

namespace vde::core::gpu
{
	class DescriptorPool
	{
		core::GraphicsContext* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;
		
	public:
		explicit DescriptorPool(util::Badge<core::GraphicsContext> badge);
		~DescriptorPool() noexcept;

		Impl& GetImpl();
		core::GraphicsContext& Owner();

		std::unique_ptr<DescriptorSet> Acquire(const DescriptorSetLayout& layout);
	};
}

#endif /* VDE__CORE__GPU__DESCRIPTORPOOL_H */
