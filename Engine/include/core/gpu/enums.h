#ifndef VDE__CORE__GPU__ENUMS_H
#define VDE__CORE__GPU__ENUMS_H
#pragma once

#include <algorithm>
#include <cstdint>

namespace vde::core::gpu
{
	enum class EImageLayout
		: uint32_t
	{
		Undefined = 0,
		General,
		ColorAttachment,
		DepthStencilAttachment,
		DepthStencilReadOnly,
		ShaderReadOnly,
		TransferSrc,
		TransferDst,
		Preinitialized,
		PresentSrc
	};

    enum class EImageFormatComponents
    {
        Undefined,
        R,
        RG,
        RGB, BGR,
        RGBA, BGRA, ARGB, ABGR,
        Depth, Stencil, DepthStencil
    };

    enum class EImageFormatType
    {
        Undefined,
        // 8-bit formats
        U4_Pack8, U8_Norm, S8_Norm, U8_Scaled, S8_Scaled, U8_Int, S8_Int, U8_UInt, S8_UInt, SRGB8,

        // 16-bit formats
        U565_Pack16, U5551_Pack16, U4444_Pack16, U16_Norm, S16_Norm, U16_Scaled, S16_Scaled, U16_Int, S16_Int, F16,

        // 32-bit formats
        U8_Norm_Pack32, S8_Norm_Pack32, U8_Scaled_Pack32, S8_Scaled_Pack32, U8_Int_Pack32, S8_Int_Pack32, X8_D24_Norm_Pack32, U32_Norm, S32_Norm, U32_Scaled, S32_Scaled, U32_Int, S32_Int, F32,

        // 64-bit formats
        U64_Norm, S64_Norm, U64_Scaled, S64_Scaled, U64_Int, S64_Int, F64,

        // Depth/Stencil formats
        D24_Norm, S8, D16_Norm_S8, D24_Norm_S8, D32_SF_S8
    };

    using ImageFormat = std::pair<EImageFormatComponents, EImageFormatType>;

	enum class EImageUsageBits
		: uint32_t
	{
		None                   = 0x00,
		TransferSrc            = 0x01,
		TransferDst            = 0x02,
		Sampled                = 0x04,
		Storage                = 0x08,
		ColorAttachment        = 0x10,
		DepthStencilAttachment = 0x20
	};

    enum class EImageType
    {
        Image1D,
        Image2D,
        Image3D,
        ImageCube,

        Undefined = -1
    };

	enum class EBufferUsageBits
        : uint32_t
	{
        TransferSrc         = 0x00000001,
        CanUpload           = 0x00000002,
        UniformBuffer       = 0x00000010,
        ShaderStorageBuffer = 0x00000020,
        IndexBuffer         = 0x00000040,
        VertexBuffer        = 0x00000080,
        IndirectBuffer      = 0x00000100
	};

	enum class EShaderStage
	{
		Vertex,
		TessControl,
		TessEval,
		Geometry,
		Fragment,
		Compute
	};

	enum class EShaderVariableType
		: uint32_t
	{
		Bool,
		Bool2,
		Bool3,
		Bool4,
		Int,
		Int2,
		Int3,
		Int4,
		UnsignedInt,
		UnsignedInt2,
		UnsignedInt3,
		UnsignedInt4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat2,
		Mat3,
		Mat4,
		Mat2x3,
		Mat2x4,
		Mat3x2,
		Mat3x4,
		Mat4x2,
		Mat4x3,
		Struct,
		Unknown
	};

    enum class EPipelineFamily
    {
        Graphics,
        Compute
    };

    enum class EDescriptorSetBindingType
    {
        UniformBuffer,
        StorageBuffer,
        ImageSampler
    };
}

extern vde::core::gpu::EImageUsageBits  operator|(vde::core::gpu::EImageUsageBits a,  vde::core::gpu::EImageUsageBits b);
extern vde::core::gpu::EImageUsageBits  operator&(vde::core::gpu::EImageUsageBits a,  vde::core::gpu::EImageUsageBits b);
extern vde::core::gpu::EBufferUsageBits operator|(vde::core::gpu::EBufferUsageBits a, vde::core::gpu::EBufferUsageBits b);
extern vde::core::gpu::EBufferUsageBits operator&(vde::core::gpu::EBufferUsageBits a, vde::core::gpu::EBufferUsageBits b);

#endif /* VDE__CORE__GPU__ENUMS_H */
