#ifndef VDE__CORE__GPU__SHADER_H
#define VDE__CORE__GPU__SHADER_H
#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>
#include <glm/glm.hpp>

#include <core/gpu/enums.h>
#include <util/badge.h>

namespace vde::core
{
	class GraphicsContext;
}

namespace vde::core::gpu
{
	class DescriptorSetLayout;
	class RenderingCommandEncoder;

	template<typename T> struct ShaderVariableTypeId;
	template<> struct ShaderVariableTypeId<bool>        { static constexpr EShaderVariableType Value = EShaderVariableType::Bool;         };
	template<> struct ShaderVariableTypeId<glm::bvec2>  { static constexpr EShaderVariableType Value = EShaderVariableType::Bool2;        };
	template<> struct ShaderVariableTypeId<glm::bvec3>  { static constexpr EShaderVariableType Value = EShaderVariableType::Bool3;        };
	template<> struct ShaderVariableTypeId<glm::bvec4>  { static constexpr EShaderVariableType Value = EShaderVariableType::Bool4;        };
	template<> struct ShaderVariableTypeId<int32_t>     { static constexpr EShaderVariableType Value = EShaderVariableType::Int;          };
	template<> struct ShaderVariableTypeId<glm::ivec2>  { static constexpr EShaderVariableType Value = EShaderVariableType::Int2;         };
	template<> struct ShaderVariableTypeId<glm::ivec3>  { static constexpr EShaderVariableType Value = EShaderVariableType::Int3;         };
	template<> struct ShaderVariableTypeId<glm::ivec4>  { static constexpr EShaderVariableType Value = EShaderVariableType::Int4;         };
	template<> struct ShaderVariableTypeId<uint32_t>    { static constexpr EShaderVariableType Value = EShaderVariableType::UnsignedInt;  };
	template<> struct ShaderVariableTypeId<glm::uvec2>  { static constexpr EShaderVariableType Value = EShaderVariableType::UnsignedInt2; };
	template<> struct ShaderVariableTypeId<glm::uvec3>  { static constexpr EShaderVariableType Value = EShaderVariableType::UnsignedInt3; };
	template<> struct ShaderVariableTypeId<glm::uvec4>  { static constexpr EShaderVariableType Value = EShaderVariableType::UnsignedInt4; };
	template<> struct ShaderVariableTypeId<float>       { static constexpr EShaderVariableType Value = EShaderVariableType::Float;        };
	template<> struct ShaderVariableTypeId<glm::vec2>   { static constexpr EShaderVariableType Value = EShaderVariableType::Float2;       };
	template<> struct ShaderVariableTypeId<glm::vec3>   { static constexpr EShaderVariableType Value = EShaderVariableType::Float3;       };
	template<> struct ShaderVariableTypeId<glm::vec4>   { static constexpr EShaderVariableType Value = EShaderVariableType::Float4;       };
	template<> struct ShaderVariableTypeId<glm::mat2>   { static constexpr EShaderVariableType Value = EShaderVariableType::Mat2;         };
	template<> struct ShaderVariableTypeId<glm::mat3>   { static constexpr EShaderVariableType Value = EShaderVariableType::Mat3;         };
	template<> struct ShaderVariableTypeId<glm::mat4>   { static constexpr EShaderVariableType Value = EShaderVariableType::Mat4;         };
	template<> struct ShaderVariableTypeId<glm::mat2x3> { static constexpr EShaderVariableType Value = EShaderVariableType::Mat2x3;       };
	template<> struct ShaderVariableTypeId<glm::mat2x4> { static constexpr EShaderVariableType Value = EShaderVariableType::Mat2x4;       };
	template<> struct ShaderVariableTypeId<glm::mat3x2> { static constexpr EShaderVariableType Value = EShaderVariableType::Mat3x2;       };
	template<> struct ShaderVariableTypeId<glm::mat3x4> { static constexpr EShaderVariableType Value = EShaderVariableType::Mat3x4;       };
	template<> struct ShaderVariableTypeId<glm::mat4x2> { static constexpr EShaderVariableType Value = EShaderVariableType::Mat4x2;       };
	template<> struct ShaderVariableTypeId<glm::mat4x3> { static constexpr EShaderVariableType Value = EShaderVariableType::Mat4x3;       };

	struct ShaderDataMember
	{
		std::string                   name;
		uint32_t                      offset;
		uint32_t                      size;
		EShaderVariableType           type;
		size_t                        arraySize      = 0;
		size_t                        arrayStride    = 0;
		bool                          isRuntimeArray = false;
		std::vector<ShaderDataMember> members;
	};

	struct ShaderDataType
	{
		uint32_t                      size;
		std::vector<ShaderDataMember> members;
	};

	class ShaderDataProxy;

	class ShaderDataStore
	{
	private:
		ShaderDataType         type;
		std::vector<std::byte> data;

	public:
		friend class ShaderDataProxy;

		ShaderDataStore() = default;
		ShaderDataStore(const ShaderDataType& shaderType, size_t bufferSize = 0);
		ShaderDataProxy operator[](const std::string& memberName);

		const ShaderDataType& Type() const;
		const void*           Data() const;
		size_t                Size() const;
	};

	class ShaderDataProxy
	{
	private:
		ShaderDataStore&       buffer;
		const ShaderDataMember* currentMember;
		bool                    isArray;
		size_t                  offset;

	public:
		ShaderDataProxy(ShaderDataStore& buf, const ShaderDataMember* member, size_t offset = 0, bool isArray = false);

		ShaderDataProxy operator[](const std::string& memberName);
		ShaderDataProxy operator[](size_t index);

		template<typename T>
		ShaderDataProxy& operator=(const T& value)
		{
			if (!currentMember || isArray || (!currentMember->members.empty()) || ShaderVariableTypeId<T>::Value != currentMember->type)
				throw std::runtime_error("Invalid value access");

			*reinterpret_cast<T*>(buffer.data.data() + offset) = value;
			return *this;
		}

		template<typename T>
		operator T()
		{
			if (!currentMember || isArray || (!currentMember->members.empty()) || ShaderVariableTypeId<T>::Value != currentMember->type)
				throw std::runtime_error("Invalid value access");

			return *reinterpret_cast<T*>(buffer.data.data() + offset);
		}
	};

	class Shader
	{
		core::GraphicsContext* m_owner;

		struct Impl;
		std::unique_ptr<Impl> m_pImpl;

	public:
		struct Info
		{
			const void* data = nullptr;
			size_t      size = 0;
		};

		struct VertexInput;
		struct PushConstantsLayout;

		explicit Shader(util::Badge<core::GraphicsContext> badge, const Info& info);
		~Shader() noexcept;

		EShaderStage Stage() const;
		Impl& GetImpl();

		VertexInput&          GetVertexInput();
		const ShaderDataType& PushConstants();
		std::vector<uint32_t> GetDescriptorSetIndices();
		DescriptorSetLayout&  GetDescriptorSetLayout(size_t set);
	};
}

#endif /* VDE__CORE__GPU__SHADER_H */
