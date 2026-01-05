#ifndef VDE__CORE__GPU__DESCRIPTORSET_H
#define VDE__CORE__GPU__DESCRIPTORSET_H
#pragma once

#include <memory>
#include <variant>

#include <core/gpu/shader.h>
#include <core/gpu/enums.h>
#include <util/badge.h>

namespace vde::core::gpu
{
	class DescriptorPool;
	class DescriptorSet;

	enum class EDescriptorSetLayoutBindingType
	{
		BlockData,
		Image
	};

	struct ImageDataType
	{
		EImageType type;
	};

	class DescriptorSetLayout
	{
		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

	public:
		struct Binding
		{
			uint32_t            bindingId = uint32_t(-1);
			std::string         name;
			std::variant<
				ShaderDataType,
				ImageDataType>  data;
		};

		uint32_t             setId = uint32_t(-1);
		std::vector<Binding> bindings;

		DescriptorSetLayout();
		DescriptorSetLayout(const DescriptorSetLayout&);
		DescriptorSetLayout& operator=(const DescriptorSetLayout&);

		Impl& GetImpl() const;

		template<typename T>
		const T& GetBinding(size_t binding);

		template<typename T>
		bool IsBinding(size_t binding)
		{
			if (binding < bindings.size())
				return std::holds_alternative<T>(bindings[binding].data);

			return false;
		}

		const std::string& GetBindingName(size_t binding);
		size_t GetBindingCount() const;
	};

	class DescriptorSet
	{
		DescriptorPool* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

	public:
		explicit DescriptorSet(util::Badge<DescriptorPool> badge, const DescriptorSetLayout& layout);
		~DescriptorSet();

		Impl& GetImpl();

		template<typename T>
		void Bind(uint32_t binding, T& target);
	};
}

#endif /* VDE__CORE__GPU__DESCRIPTORSET_H */
