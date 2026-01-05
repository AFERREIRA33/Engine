#include "vk_converters.h"

VkImageLayout s_ToVk(vde::core::gpu::EImageLayout layout)
{
    switch (layout)
    {
    case vde::core::gpu::EImageLayout::Undefined: 			   return VK_IMAGE_LAYOUT_UNDEFINED;
    case vde::core::gpu::EImageLayout::General: 			   return VK_IMAGE_LAYOUT_GENERAL;
    case vde::core::gpu::EImageLayout::ColorAttachment: 	   return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case vde::core::gpu::EImageLayout::DepthStencilAttachment: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    case vde::core::gpu::EImageLayout::DepthStencilReadOnly:   return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    case vde::core::gpu::EImageLayout::ShaderReadOnly: 		   return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    case vde::core::gpu::EImageLayout::TransferSrc: 		   return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case vde::core::gpu::EImageLayout::TransferDst: 		   return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    case vde::core::gpu::EImageLayout::Preinitialized: 		   return VK_IMAGE_LAYOUT_PREINITIALIZED;
    case vde::core::gpu::EImageLayout::PresentSrc:             return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }

    return VkImageLayout(-1);
}

size_t s_ToVkComponentSize(VkFormat value)
{
    switch (value)
    {
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8_SNORM:
    case VK_FORMAT_R8_UINT:
    case VK_FORMAT_R8_SINT:
    case VK_FORMAT_R8_USCALED:
    case VK_FORMAT_R8_SSCALED:
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8_SNORM:
    case VK_FORMAT_R8G8_UINT:
    case VK_FORMAT_R8G8_SINT:
    case VK_FORMAT_R8G8_USCALED:
    case VK_FORMAT_R8G8_SSCALED:
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8_SNORM:
    case VK_FORMAT_R8G8B8_UINT:
    case VK_FORMAT_R8G8B8_SINT:
    case VK_FORMAT_R8G8B8_USCALED:
    case VK_FORMAT_R8G8B8_SSCALED:
    case VK_FORMAT_R8G8B8_SRGB:
    case VK_FORMAT_B8G8R8_UNORM:
    case VK_FORMAT_B8G8R8_SNORM:
    case VK_FORMAT_B8G8R8_UINT:
    case VK_FORMAT_B8G8R8_SINT:
    case VK_FORMAT_B8G8R8_USCALED:
    case VK_FORMAT_B8G8R8_SSCALED:
    case VK_FORMAT_B8G8R8_SRGB:
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SNORM:
    case VK_FORMAT_R8G8B8A8_UINT:
    case VK_FORMAT_R8G8B8A8_SINT:
    case VK_FORMAT_R8G8B8A8_USCALED:
    case VK_FORMAT_R8G8B8A8_SSCALED:
    case VK_FORMAT_R8G8B8A8_SRGB:
    case VK_FORMAT_B8G8R8A8_UNORM:
    case VK_FORMAT_B8G8R8A8_SNORM:
    case VK_FORMAT_B8G8R8A8_UINT:
    case VK_FORMAT_B8G8R8A8_SINT:
    case VK_FORMAT_B8G8R8A8_USCALED:
    case VK_FORMAT_B8G8R8A8_SSCALED:
    case VK_FORMAT_B8G8R8A8_SRGB:
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        return 1;

    case VK_FORMAT_R16_UNORM:
    case VK_FORMAT_R16_SNORM:
    case VK_FORMAT_R16_UINT:
    case VK_FORMAT_R16_SINT:
    case VK_FORMAT_R16_USCALED:
    case VK_FORMAT_R16_SSCALED:
    case VK_FORMAT_R16_SFLOAT:
    case VK_FORMAT_R16G16_UNORM:
    case VK_FORMAT_R16G16_SNORM:
    case VK_FORMAT_R16G16_UINT:
    case VK_FORMAT_R16G16_SINT:
    case VK_FORMAT_R16G16_USCALED:
    case VK_FORMAT_R16G16_SSCALED:
    case VK_FORMAT_R16G16_SFLOAT:
    case VK_FORMAT_R16G16B16_UNORM:
    case VK_FORMAT_R16G16B16_SNORM:
    case VK_FORMAT_R16G16B16_UINT:
    case VK_FORMAT_R16G16B16_SINT:
    case VK_FORMAT_R16G16B16_USCALED:
    case VK_FORMAT_R16G16B16_SSCALED:
    case VK_FORMAT_R16G16B16_SFLOAT:
    case VK_FORMAT_R16G16B16A16_UNORM:
    case VK_FORMAT_R16G16B16A16_SNORM:
    case VK_FORMAT_R16G16B16A16_UINT:
    case VK_FORMAT_R16G16B16A16_SINT:
    case VK_FORMAT_R16G16B16A16_USCALED:
    case VK_FORMAT_R16G16B16A16_SSCALED:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return 2;

    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SINT:
    case VK_FORMAT_R32_SFLOAT:
    case VK_FORMAT_R32G32_UINT:
    case VK_FORMAT_R32G32_SINT:
    case VK_FORMAT_R32G32_SFLOAT:
    case VK_FORMAT_R32G32B32_UINT:
    case VK_FORMAT_R32G32B32_SINT:
    case VK_FORMAT_R32G32B32_SFLOAT:
    case VK_FORMAT_R32G32B32A32_UINT:
    case VK_FORMAT_R32G32B32A32_SINT:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return 4;

    case VK_FORMAT_R64_SFLOAT:
    case VK_FORMAT_R64G64_SFLOAT:
    case VK_FORMAT_R64G64B64_SFLOAT:
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return 0;

    default:
        break;
    }

    return 0;
}

size_t s_ToVkComponentCount(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8_SNORM:
    case VK_FORMAT_R8_UINT:
    case VK_FORMAT_R8_SINT:
    case VK_FORMAT_R8_USCALED:
    case VK_FORMAT_R8_SSCALED:
    case VK_FORMAT_R16_UNORM:
    case VK_FORMAT_R16_SNORM:
    case VK_FORMAT_R16_UINT:
    case VK_FORMAT_R16_SINT:
    case VK_FORMAT_R16_USCALED:
    case VK_FORMAT_R16_SSCALED:
    case VK_FORMAT_R16_SFLOAT:
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SINT:
    case VK_FORMAT_R32_SFLOAT:
    case VK_FORMAT_R64_SFLOAT:
        return 1;

    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8_SNORM:
    case VK_FORMAT_R8G8_UINT:
    case VK_FORMAT_R8G8_SINT:
    case VK_FORMAT_R8G8_USCALED:
    case VK_FORMAT_R8G8_SSCALED:
    case VK_FORMAT_R16G16_UNORM:
    case VK_FORMAT_R16G16_SNORM:
    case VK_FORMAT_R16G16_UINT:
    case VK_FORMAT_R16G16_SINT:
    case VK_FORMAT_R16G16_USCALED:
    case VK_FORMAT_R16G16_SSCALED:
    case VK_FORMAT_R16G16_SFLOAT:
    case VK_FORMAT_R32G32_UINT:
    case VK_FORMAT_R32G32_SINT:
    case VK_FORMAT_R32G32_SFLOAT:
    case VK_FORMAT_R64G64_SFLOAT:
        return 2;

    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8_SNORM:
    case VK_FORMAT_R8G8B8_UINT:
    case VK_FORMAT_R8G8B8_SINT:
    case VK_FORMAT_R8G8B8_USCALED:
    case VK_FORMAT_R8G8B8_SSCALED:
    case VK_FORMAT_R8G8B8_SRGB:
    case VK_FORMAT_R16G16B16_UNORM:
    case VK_FORMAT_R16G16B16_SNORM:
    case VK_FORMAT_R16G16B16_UINT:
    case VK_FORMAT_R16G16B16_SINT:
    case VK_FORMAT_R16G16B16_USCALED:
    case VK_FORMAT_R16G16B16_SSCALED:
    case VK_FORMAT_R16G16B16_SFLOAT:
    case VK_FORMAT_R32G32B32_UINT:
    case VK_FORMAT_R32G32B32_SINT:
    case VK_FORMAT_R32G32B32_SFLOAT:
    case VK_FORMAT_R64G64B64_SFLOAT:
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
    case VK_FORMAT_B8G8R8_UNORM:
    case VK_FORMAT_B8G8R8_SNORM:
    case VK_FORMAT_B8G8R8_UINT:
    case VK_FORMAT_B8G8R8_SINT:
    case VK_FORMAT_B8G8R8_USCALED:
    case VK_FORMAT_B8G8R8_SSCALED:
    case VK_FORMAT_B8G8R8_SRGB:
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
        return 3;

    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SNORM:
    case VK_FORMAT_R8G8B8A8_UINT:
    case VK_FORMAT_R8G8B8A8_SINT:
    case VK_FORMAT_R8G8B8A8_USCALED:
    case VK_FORMAT_R8G8B8A8_SSCALED:
    case VK_FORMAT_R8G8B8A8_SRGB:
    case VK_FORMAT_R16G16B16A16_UNORM:
    case VK_FORMAT_R16G16B16A16_SNORM:
    case VK_FORMAT_R16G16B16A16_UINT:
    case VK_FORMAT_R16G16B16A16_SINT:
    case VK_FORMAT_R16G16B16A16_USCALED:
    case VK_FORMAT_R16G16B16A16_SSCALED:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_R32G32B32A32_UINT:
    case VK_FORMAT_R32G32B32A32_SINT:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
    case VK_FORMAT_R64G64B64A64_SFLOAT:
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
    case VK_FORMAT_B8G8R8A8_UNORM:
    case VK_FORMAT_B8G8R8A8_SNORM:
    case VK_FORMAT_B8G8R8A8_UINT:
    case VK_FORMAT_B8G8R8A8_SINT:
    case VK_FORMAT_B8G8R8A8_USCALED:
    case VK_FORMAT_B8G8R8A8_SSCALED:
    case VK_FORMAT_B8G8R8A8_SRGB:
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
        return 4;

    default:
        break;
    }

    return 0;
}

size_t s_ToVkPixelSize(VkFormat format)
{
    return s_ToVkComponentCount(format) * s_ToVkComponentSize(format);
}

VkFormat s_ToVk(const vde::core::gpu::ImageFormat& value)
{
    using Comp = vde::core::gpu::EImageFormatComponents;
    using Type = vde::core::gpu::EImageFormatType;

    auto components = value.first;
    auto type = value.second;

    if (components == Comp::Undefined && type == Type::Undefined)
        return VK_FORMAT_UNDEFINED;

    if (components == Comp::Depth)
    {
        if (type == Type::U16_Norm) return VK_FORMAT_D16_UNORM;
        if (type == Type::F32)      return VK_FORMAT_D32_SFLOAT;
    }

    if (components == Comp::Stencil)
    {
        if (type == Type::S8) return VK_FORMAT_S8_UINT;
    }

    if (components == Comp::DepthStencil)
    {
        if (type == Type::D16_Norm_S8) return VK_FORMAT_D16_UNORM_S8_UINT;
        if (type == Type::D24_Norm_S8) return VK_FORMAT_D24_UNORM_S8_UINT;
        if (type == Type::D32_SF_S8)   return VK_FORMAT_D32_SFLOAT_S8_UINT;
    }

    switch (components)
    {
    case Comp::R:
        switch (type)
        {
        case Type::U8_Norm:    return VK_FORMAT_R8_UNORM;
        case Type::S8_Norm:    return VK_FORMAT_R8_SNORM;
        case Type::U8_UInt:    return VK_FORMAT_R8_UINT;
        case Type::S8_Int:     return VK_FORMAT_R8_SINT;
        case Type::U8_Scaled:  return VK_FORMAT_R8_USCALED;
        case Type::S8_Scaled:  return VK_FORMAT_R8_SSCALED;
        case Type::U16_Norm:   return VK_FORMAT_R16_UNORM;
        case Type::S16_Norm:   return VK_FORMAT_R16_SNORM;
        case Type::U16_Int:    return VK_FORMAT_R16_UINT;
        case Type::S16_Int:    return VK_FORMAT_R16_SINT;
        case Type::U16_Scaled: return VK_FORMAT_R16_USCALED;
        case Type::S16_Scaled: return VK_FORMAT_R16_SSCALED;
        case Type::F16:        return VK_FORMAT_R16_SFLOAT;
        case Type::U32_Int:    return VK_FORMAT_R32_UINT;
        case Type::S32_Int:    return VK_FORMAT_R32_SINT;
        case Type::F32:        return VK_FORMAT_R32_SFLOAT;
        case Type::F64:        return VK_FORMAT_R64_SFLOAT;
        default: break;
        }
        break;

    case Comp::RG:
        switch (type)
        {
        case Type::U8_Norm:    return VK_FORMAT_R8G8_UNORM;
        case Type::S8_Norm:    return VK_FORMAT_R8G8_SNORM;
        case Type::U8_UInt:    return VK_FORMAT_R8G8_UINT;
        case Type::S8_Int:     return VK_FORMAT_R8G8_SINT;
        case Type::U8_Scaled:  return VK_FORMAT_R8G8_USCALED;
        case Type::S8_Scaled:  return VK_FORMAT_R8G8_SSCALED;
        case Type::U16_Norm:   return VK_FORMAT_R16G16_UNORM;
        case Type::S16_Norm:   return VK_FORMAT_R16G16_SNORM;
        case Type::U16_Int:    return VK_FORMAT_R16G16_UINT;
        case Type::S16_Int:    return VK_FORMAT_R16G16_SINT;
        case Type::U16_Scaled: return VK_FORMAT_R16G16_USCALED;
        case Type::S16_Scaled: return VK_FORMAT_R16G16_SSCALED;
        case Type::F16:        return VK_FORMAT_R16G16_SFLOAT;
        case Type::U32_Int:    return VK_FORMAT_R32G32_UINT;
        case Type::S32_Int:    return VK_FORMAT_R32G32_SINT;
        case Type::F32:        return VK_FORMAT_R32G32_SFLOAT;
        case Type::F64:        return VK_FORMAT_R64G64_SFLOAT;
        default: break;
        }
        break;

    case Comp::RGB:
        switch (type)
        {
        case Type::U8_Norm:     return VK_FORMAT_R8G8B8_UNORM;
        case Type::S8_Norm:     return VK_FORMAT_R8G8B8_SNORM;
        case Type::U8_UInt:     return VK_FORMAT_R8G8B8_UINT;
        case Type::S8_Int:      return VK_FORMAT_R8G8B8_SINT;
        case Type::U8_Scaled:   return VK_FORMAT_R8G8B8_USCALED;
        case Type::S8_Scaled:   return VK_FORMAT_R8G8B8_SSCALED;
        case Type::SRGB8:       return VK_FORMAT_R8G8B8_SRGB;
        case Type::U16_Norm:    return VK_FORMAT_R16G16B16_UNORM;
        case Type::S16_Norm:    return VK_FORMAT_R16G16B16_SNORM;
        case Type::U16_Int:     return VK_FORMAT_R16G16B16_UINT;
        case Type::S16_Int:     return VK_FORMAT_R16G16B16_SINT;
        case Type::U16_Scaled:  return VK_FORMAT_R16G16B16_USCALED;
        case Type::S16_Scaled:  return VK_FORMAT_R16G16B16_SSCALED;
        case Type::F16:         return VK_FORMAT_R16G16B16_SFLOAT;
        case Type::U32_Int:     return VK_FORMAT_R32G32B32_UINT;
        case Type::S32_Int:     return VK_FORMAT_R32G32B32_SINT;
        case Type::F32:         return VK_FORMAT_R32G32B32_SFLOAT;
        case Type::F64:         return VK_FORMAT_R64G64B64_SFLOAT;
        case Type::U565_Pack16: return VK_FORMAT_R5G6B5_UNORM_PACK16;
        default: break;
        }
        break;

    case Comp::BGR:
        switch (type)
        {
        case Type::U8_Norm:     return VK_FORMAT_B8G8R8_UNORM;
        case Type::S8_Norm:     return VK_FORMAT_B8G8R8_SNORM;
        case Type::U8_UInt:     return VK_FORMAT_B8G8R8_UINT;
        case Type::S8_Int:      return VK_FORMAT_B8G8R8_SINT;
        case Type::U8_Scaled:   return VK_FORMAT_B8G8R8_USCALED;
        case Type::S8_Scaled:   return VK_FORMAT_B8G8R8_SSCALED;
        case Type::SRGB8:       return VK_FORMAT_B8G8R8_SRGB;
        case Type::U565_Pack16: return VK_FORMAT_B5G6R5_UNORM_PACK16;
        default: break;
        }
        break;

    case Comp::RGBA:
        switch (type)
        {
        case Type::U8_Norm:      return VK_FORMAT_R8G8B8A8_UNORM;
        case Type::S8_Norm:      return VK_FORMAT_R8G8B8A8_SNORM;
        case Type::U8_UInt:      return VK_FORMAT_R8G8B8A8_UINT;
        case Type::S8_Int:       return VK_FORMAT_R8G8B8A8_SINT;
        case Type::U8_Scaled:    return VK_FORMAT_R8G8B8A8_USCALED;
        case Type::S8_Scaled:    return VK_FORMAT_R8G8B8A8_SSCALED;
        case Type::SRGB8:        return VK_FORMAT_R8G8B8A8_SRGB;
        case Type::U16_Norm:     return VK_FORMAT_R16G16B16A16_UNORM;
        case Type::S16_Norm:     return VK_FORMAT_R16G16B16A16_SNORM;
        case Type::U16_Int:      return VK_FORMAT_R16G16B16A16_UINT;
        case Type::S16_Int:      return VK_FORMAT_R16G16B16A16_SINT;
        case Type::U16_Scaled:   return VK_FORMAT_R16G16B16A16_USCALED;
        case Type::S16_Scaled:   return VK_FORMAT_R16G16B16A16_SSCALED;
        case Type::F16:          return VK_FORMAT_R16G16B16A16_SFLOAT;
        case Type::U32_Int:      return VK_FORMAT_R32G32B32A32_UINT;
        case Type::S32_Int:      return VK_FORMAT_R32G32B32A32_SINT;
        case Type::F32:          return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Type::F64:          return VK_FORMAT_R64G64B64A64_SFLOAT;
        case Type::U4444_Pack16: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case Type::U5551_Pack16: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
        default: break;
        }
        break;

    case Comp::BGRA:
        switch (type)
        {
        case Type::U8_Norm:      return VK_FORMAT_B8G8R8A8_UNORM;
        case Type::S8_Norm:      return VK_FORMAT_B8G8R8A8_SNORM;
        case Type::U8_UInt:      return VK_FORMAT_B8G8R8A8_UINT;
        case Type::S8_Int:       return VK_FORMAT_B8G8R8A8_SINT;
        case Type::U8_Scaled:    return VK_FORMAT_B8G8R8A8_USCALED;
        case Type::S8_Scaled:    return VK_FORMAT_B8G8R8A8_SSCALED;
        case Type::SRGB8:        return VK_FORMAT_B8G8R8A8_SRGB;
        case Type::U4444_Pack16: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
        case Type::U5551_Pack16: return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
        default: break;
        }
        break;

    case Comp::ABGR:
        switch (type)
        {
        case Type::U8_Norm_Pack32:   return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
        case Type::S8_Norm_Pack32:   return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
        case Type::U8_UInt:          return VK_FORMAT_A8B8G8R8_UINT_PACK32;
        case Type::S8_Int:           return VK_FORMAT_A8B8G8R8_SINT_PACK32;
        case Type::U8_Scaled_Pack32: return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
        case Type::S8_Scaled_Pack32: return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
        case Type::SRGB8:            return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
        default: break;
        }
        break;

    case Comp::ARGB:
        switch (type)
        {
        case Type::U4_Pack8: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
        default: break;
        }
        break;

    default:
        break;
    }

    return VK_FORMAT_UNDEFINED;
}

vde::core::gpu::ImageFormat s_ToVde(VkFormat format)
{
    using Comp = vde::core::gpu::EImageFormatComponents;
    using Type = vde::core::gpu::EImageFormatType;

    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return { Comp::Undefined, Type::Undefined };

    // Depth formats
    case VK_FORMAT_D16_UNORM:  return { Comp::Depth, Type::U16_Norm };
    case VK_FORMAT_D32_SFLOAT: return { Comp::Depth, Type::F32 };

    // Stencil formats
    case VK_FORMAT_S8_UINT: return { Comp::Stencil, Type::S8 };

    // Depth/Stencil formats
    case VK_FORMAT_D16_UNORM_S8_UINT:  return { Comp::DepthStencil, Type::D16_Norm_S8 };
    case VK_FORMAT_D24_UNORM_S8_UINT:  return { Comp::DepthStencil, Type::D24_Norm_S8 };
    case VK_FORMAT_D32_SFLOAT_S8_UINT: return { Comp::DepthStencil, Type::D32_SF_S8 };

    // R formats
    case VK_FORMAT_R8_UNORM:    return { Comp::R, Type::U8_Norm };
    case VK_FORMAT_R8_SNORM:    return { Comp::R, Type::S8_Norm };
    case VK_FORMAT_R8_UINT:     return { Comp::R, Type::U8_UInt };
    case VK_FORMAT_R8_SINT:     return { Comp::R, Type::S8_Int };
    case VK_FORMAT_R8_USCALED:  return { Comp::R, Type::U8_Scaled };
    case VK_FORMAT_R8_SSCALED:  return { Comp::R, Type::S8_Scaled };
    case VK_FORMAT_R16_UNORM:   return { Comp::R, Type::U16_Norm };
    case VK_FORMAT_R16_SNORM:   return { Comp::R, Type::S16_Norm };
    case VK_FORMAT_R16_UINT:    return { Comp::R, Type::U16_Int };
    case VK_FORMAT_R16_SINT:    return { Comp::R, Type::S16_Int };
    case VK_FORMAT_R16_USCALED: return { Comp::R, Type::U16_Scaled };
    case VK_FORMAT_R16_SSCALED: return { Comp::R, Type::S16_Scaled };
    case VK_FORMAT_R16_SFLOAT:  return { Comp::R, Type::F16 };
    case VK_FORMAT_R32_UINT:    return { Comp::R, Type::U32_Int };
    case VK_FORMAT_R32_SINT:    return { Comp::R, Type::S32_Int };
    case VK_FORMAT_R32_SFLOAT:  return { Comp::R, Type::F32 };
    case VK_FORMAT_R64_SFLOAT:  return { Comp::R, Type::F64 };

    // RG formats
    case VK_FORMAT_R8G8_UNORM:     return { Comp::RG, Type::U8_Norm };
    case VK_FORMAT_R8G8_SNORM:     return { Comp::RG, Type::S8_Norm };
    case VK_FORMAT_R8G8_UINT:      return { Comp::RG, Type::U8_UInt };
    case VK_FORMAT_R8G8_SINT:      return { Comp::RG, Type::S8_Int };
    case VK_FORMAT_R8G8_USCALED:   return { Comp::RG, Type::U8_Scaled };
    case VK_FORMAT_R8G8_SSCALED:   return { Comp::RG, Type::S8_Scaled };
    case VK_FORMAT_R16G16_UNORM:   return { Comp::RG, Type::U16_Norm };
    case VK_FORMAT_R16G16_SNORM:   return { Comp::RG, Type::S16_Norm };
    case VK_FORMAT_R16G16_UINT:    return { Comp::RG, Type::U16_Int };
    case VK_FORMAT_R16G16_SINT:    return { Comp::RG, Type::S16_Int };
    case VK_FORMAT_R16G16_USCALED: return { Comp::RG, Type::U16_Scaled };
    case VK_FORMAT_R16G16_SSCALED: return { Comp::RG, Type::S16_Scaled };
    case VK_FORMAT_R16G16_SFLOAT:  return { Comp::RG, Type::F16 };
    case VK_FORMAT_R32G32_UINT:    return { Comp::RG, Type::U32_Int };
    case VK_FORMAT_R32G32_SINT:    return { Comp::RG, Type::S32_Int };
    case VK_FORMAT_R32G32_SFLOAT:  return { Comp::RG, Type::F32 };
    case VK_FORMAT_R64G64_SFLOAT:  return { Comp::RG, Type::F64 };

    // RGB formats
    case VK_FORMAT_R8G8B8_UNORM:        return { Comp::RGB, Type::U8_Norm };
    case VK_FORMAT_R8G8B8_SNORM:        return { Comp::RGB, Type::S8_Norm };
    case VK_FORMAT_R8G8B8_UINT:         return { Comp::RGB, Type::U8_UInt };
    case VK_FORMAT_R8G8B8_SINT:         return { Comp::RGB, Type::S8_Int };
    case VK_FORMAT_R8G8B8_USCALED:      return { Comp::RGB, Type::U8_Scaled };
    case VK_FORMAT_R8G8B8_SSCALED:      return { Comp::RGB, Type::S8_Scaled };
    case VK_FORMAT_R8G8B8_SRGB:         return { Comp::RGB, Type::SRGB8 };
    case VK_FORMAT_R16G16B16_UNORM:     return { Comp::RGB, Type::U16_Norm };
    case VK_FORMAT_R16G16B16_SNORM:     return { Comp::RGB, Type::S16_Norm };
    case VK_FORMAT_R16G16B16_UINT:      return { Comp::RGB, Type::U16_Int };
    case VK_FORMAT_R16G16B16_SINT:      return { Comp::RGB, Type::S16_Int };
    case VK_FORMAT_R16G16B16_USCALED:   return { Comp::RGB, Type::U16_Scaled };
    case VK_FORMAT_R16G16B16_SSCALED:   return { Comp::RGB, Type::S16_Scaled };
    case VK_FORMAT_R16G16B16_SFLOAT:    return { Comp::RGB, Type::F16 };
    case VK_FORMAT_R32G32B32_UINT:      return { Comp::RGB, Type::U32_Int };
    case VK_FORMAT_R32G32B32_SINT:      return { Comp::RGB, Type::S32_Int };
    case VK_FORMAT_R32G32B32_SFLOAT:    return { Comp::RGB, Type::F32 };
    case VK_FORMAT_R64G64B64_SFLOAT:    return { Comp::RGB, Type::F64 };
    case VK_FORMAT_R5G6B5_UNORM_PACK16: return { Comp::RGB, Type::U565_Pack16 };

    // BGR formats
    case VK_FORMAT_B8G8R8_UNORM:        return { Comp::BGR, Type::U8_Norm };
    case VK_FORMAT_B8G8R8_SNORM:        return { Comp::BGR, Type::S8_Norm };
    case VK_FORMAT_B8G8R8_UINT:         return { Comp::BGR, Type::U8_UInt };
    case VK_FORMAT_B8G8R8_SINT:         return { Comp::BGR, Type::S8_Int };
    case VK_FORMAT_B8G8R8_USCALED:      return { Comp::BGR, Type::U8_Scaled };
    case VK_FORMAT_B8G8R8_SSCALED:      return { Comp::BGR, Type::S8_Scaled };
    case VK_FORMAT_B8G8R8_SRGB:         return { Comp::BGR, Type::SRGB8 };
    case VK_FORMAT_B5G6R5_UNORM_PACK16: return { Comp::BGR, Type::U565_Pack16 };

    // RGBA formats
    case VK_FORMAT_R8G8B8A8_UNORM:        return { Comp::RGBA, Type::U8_Norm };
    case VK_FORMAT_R8G8B8A8_SNORM:        return { Comp::RGBA, Type::S8_Norm };
    case VK_FORMAT_R8G8B8A8_UINT:         return { Comp::RGBA, Type::U8_UInt };
    case VK_FORMAT_R8G8B8A8_SINT:         return { Comp::RGBA, Type::S8_Int };
    case VK_FORMAT_R8G8B8A8_USCALED:      return { Comp::RGBA, Type::U8_Scaled };
    case VK_FORMAT_R8G8B8A8_SSCALED:      return { Comp::RGBA, Type::S8_Scaled };
    case VK_FORMAT_R8G8B8A8_SRGB:         return { Comp::RGBA, Type::SRGB8 };
    case VK_FORMAT_R16G16B16A16_UNORM:    return { Comp::RGBA, Type::U16_Norm };
    case VK_FORMAT_R16G16B16A16_SNORM:    return { Comp::RGBA, Type::S16_Norm };
    case VK_FORMAT_R16G16B16A16_UINT:     return { Comp::RGBA, Type::U16_Int };
    case VK_FORMAT_R16G16B16A16_SINT:     return { Comp::RGBA, Type::S16_Int };
    case VK_FORMAT_R16G16B16A16_USCALED:  return { Comp::RGBA, Type::U16_Scaled };
    case VK_FORMAT_R16G16B16A16_SSCALED:  return { Comp::RGBA, Type::S16_Scaled };
    case VK_FORMAT_R16G16B16A16_SFLOAT:   return { Comp::RGBA, Type::F16 };
    case VK_FORMAT_R32G32B32A32_UINT:     return { Comp::RGBA, Type::U32_Int };
    case VK_FORMAT_R32G32B32A32_SINT:     return { Comp::RGBA, Type::S32_Int };
    case VK_FORMAT_R32G32B32A32_SFLOAT:   return { Comp::RGBA, Type::F32 };
    case VK_FORMAT_R64G64B64A64_SFLOAT:   return { Comp::RGBA, Type::F64 };
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return { Comp::RGBA, Type::U4444_Pack16 };
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return { Comp::RGBA, Type::U5551_Pack16 };

    // BGRA formats
    case VK_FORMAT_B8G8R8A8_UNORM:        return { Comp::BGRA, Type::U8_Norm };
    case VK_FORMAT_B8G8R8A8_SNORM:        return { Comp::BGRA, Type::S8_Norm };
    case VK_FORMAT_B8G8R8A8_UINT:         return { Comp::BGRA, Type::U8_UInt };
    case VK_FORMAT_B8G8R8A8_SINT:         return { Comp::BGRA, Type::S8_Int };
    case VK_FORMAT_B8G8R8A8_USCALED:      return { Comp::BGRA, Type::U8_Scaled };
    case VK_FORMAT_B8G8R8A8_SSCALED:      return { Comp::BGRA, Type::S8_Scaled };
    case VK_FORMAT_B8G8R8A8_SRGB:         return { Comp::BGRA, Type::SRGB8 };
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return { Comp::BGRA, Type::U4444_Pack16 };
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return { Comp::BGRA, Type::U5551_Pack16 };

    // ABGR formats
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:   return { Comp::ABGR, Type::U8_Norm_Pack32 };
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:   return { Comp::ABGR, Type::S8_Norm_Pack32 };
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:    return { Comp::ABGR, Type::U8_UInt };
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:    return { Comp::ABGR, Type::S8_Int };
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return { Comp::ABGR, Type::U8_Scaled_Pack32 };
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return { Comp::ABGR, Type::S8_Scaled_Pack32 };
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:    return { Comp::ABGR, Type::SRGB8 };

    // ARGB formats
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return { Comp::ARGB, Type::U4_Pack8 };

    default:
        return { Comp::Undefined, Type::Undefined };
    }
}

VkImageUsageFlags s_ToVk(vde::core::gpu::EImageUsageBits bits)
{
    VkImageUsageFlags result{};

    if ((bits & vde::core::gpu::EImageUsageBits::TransferSrc) != vde::core::gpu::EImageUsageBits::None)            result = result | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if ((bits & vde::core::gpu::EImageUsageBits::TransferDst) != vde::core::gpu::EImageUsageBits::None)            result = result | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if ((bits & vde::core::gpu::EImageUsageBits::Sampled) != vde::core::gpu::EImageUsageBits::None)                result = result | VK_IMAGE_USAGE_SAMPLED_BIT;
    if ((bits & vde::core::gpu::EImageUsageBits::Storage) != vde::core::gpu::EImageUsageBits::None)                result = result | VK_IMAGE_USAGE_STORAGE_BIT;
    if ((bits & vde::core::gpu::EImageUsageBits::ColorAttachment) != vde::core::gpu::EImageUsageBits::None)        result = result | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if ((bits & vde::core::gpu::EImageUsageBits::DepthStencilAttachment) != vde::core::gpu::EImageUsageBits::None) result = result | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    return result;
}

uint32_t s_ToVkStride(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_R16_UINT:
    case VK_FORMAT_R16_SINT:
    case VK_FORMAT_R16_SFLOAT:
        return 2;

    case VK_FORMAT_R16G16_UINT:
    case VK_FORMAT_R16G16_SINT:
    case VK_FORMAT_R16G16_SFLOAT:
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SINT:
    case VK_FORMAT_R32_SFLOAT:
        return 4;

    case VK_FORMAT_R16G16B16_UINT:
    case VK_FORMAT_R16G16B16_SINT:
    case VK_FORMAT_R16G16B16_SFLOAT:
        return 6;

    case VK_FORMAT_R16G16B16A16_UINT:
    case VK_FORMAT_R16G16B16A16_SINT:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
    case VK_FORMAT_R32G32_UINT:
    case VK_FORMAT_R32G32_SINT:
    case VK_FORMAT_R32G32_SFLOAT:
    case VK_FORMAT_R64_UINT:
    case VK_FORMAT_R64_SINT:
    case VK_FORMAT_R64_SFLOAT:
        return 8;

    case VK_FORMAT_R32G32B32_UINT:
    case VK_FORMAT_R32G32B32_SINT:
    case VK_FORMAT_R32G32B32_SFLOAT:
        return 12;

    case VK_FORMAT_R32G32B32A32_UINT:
    case VK_FORMAT_R32G32B32A32_SINT:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
    case VK_FORMAT_R64G64_UINT:
    case VK_FORMAT_R64G64_SINT:
    case VK_FORMAT_R64G64_SFLOAT:
        return 16;

    case VK_FORMAT_R64G64B64_UINT:
    case VK_FORMAT_R64G64B64_SINT:
    case VK_FORMAT_R64G64B64_SFLOAT:
        return 24;

    case VK_FORMAT_R64G64B64A64_UINT:
    case VK_FORMAT_R64G64B64A64_SINT:
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return 32;

    case VK_FORMAT_UNDEFINED:
    default:
        break;
    }
    return 0;
}

std::tuple<VkImageType, VkImageViewType, VkImageCreateFlags> s_ToVk(vde::core::gpu::EImageType type, uint32_t arrayLayers = 1)
{
    bool isArray = (arrayLayers > 1);

    switch (type)
    {
    case vde::core::gpu::EImageType::Image1D:   return { VK_IMAGE_TYPE_1D, isArray ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D,           0 };
    case vde::core::gpu::EImageType::Image2D:   return { VK_IMAGE_TYPE_2D, isArray ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,           0 };
    case vde::core::gpu::EImageType::Image3D:   return { VK_IMAGE_TYPE_3D, VK_IMAGE_VIEW_TYPE_3D,                                                   0 };
    case vde::core::gpu::EImageType::ImageCube: return { VK_IMAGE_TYPE_2D, isArray ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE,       VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT };
    default:
        break;
    }

    return { VK_IMAGE_TYPE_MAX_ENUM, VK_IMAGE_VIEW_TYPE_MAX_ENUM, 0 };
}
