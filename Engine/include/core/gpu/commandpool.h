#ifndef VDE__CORE__GPU__COMMANDPOOL_H
#define VDE__CORE__GPU__COMMANDPOOL_H
#pragma once

#include <memory>
#include <unordered_map>
#include <util/badge.h>

#include <glm/glm.hpp>

namespace vde::core
{
	class GraphicsContext;
}

namespace vde::core::gpu
{
	class CommandBuffer;

	class CommandPool
	{
		GraphicsContext* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;
		
		std::unordered_map<CommandBuffer*, bool> m_commandBuffers;
		std::unordered_map<CommandBuffer*, bool> m_secondaryCommandBuffers;

	public:
		explicit CommandPool(util::Badge<core::GraphicsContext> badge);
		~CommandPool() noexcept;

		Impl& GetImpl();
		core::GraphicsContext& Owner();

		CommandBuffer& Acquire(bool isSecondary = false);
		void Release(CommandBuffer& cb);
		void Purge();
	};
}

#endif /* VDE__CORE__GPU__COMMANDPOOL_H */
