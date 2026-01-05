#ifndef VDE__CORE__ENGINE__BUFFER_H
#define VDE__CORE__ENGINE__BUFFER_H
#pragma once

#include <cstdint>
#include <memory>

#include <core/gpu/enums.h>
#include <core/gpu/shader.h>
#include <util/badge.h>

namespace vde::core
{
	class GraphicsContext;
}

namespace vde::core::gpu
{
	class Buffer
	{
		core::GraphicsContext* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

	public:
		struct Info
		{
			size_t           sizeInBytes;
			EBufferUsageBits usage;

			const void*      initialData = nullptr;
		};

		explicit Buffer(util::Badge<core::GraphicsContext> badge, const Info& info);
		~Buffer() noexcept;

		Impl& GetImpl();
		size_t Size() const;

		void Upload(const void* data, size_t size);
		void Upload(const ShaderDataStore& dataStore);

		template<typename T, size_t N>
		void Upload(const std::array<T, N>& data)
		{
			Upload(data.data(), data.size() * sizeof(T));
		}
	};
}

#endif /* VDE__CORE__ENGINE__BUFFER_H */
