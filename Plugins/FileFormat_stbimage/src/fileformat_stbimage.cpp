#include <fileformat_stbimage.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

FileFormat_stbimage::FileFormat_stbimage(vde::core::GraphicsContext* graphicsContext)
	: m_graphicsContext(graphicsContext)
{
}

bool FileFormat_stbimage::CanLoad(const std::string& path)
{
	if (path.ends_with(".jpg") || path.ends_with(".jpeg")) return true;
	if (path.ends_with(".png"))                            return true;
	return false;
}

std::unique_ptr<vde::core::gpu::Image> FileFormat_stbimage::Load(const void* data, size_t size)
{
	int w, h, comp;

	stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<stbi_uc const*>(data), int(size), &w, &h, &comp, 4);
	std::unique_ptr<vde::core::gpu::Image> result = m_graphicsContext->CreateImage({
		vde::core::gpu::EImageType::Image2D,
		{ w, h, 1 },
		{ vde::core::gpu::EImageFormatComponents::RGBA, vde::core::gpu::EImageFormatType::U8_Norm },
		vde::core::gpu::EImageUsageBits::Sampled,
		1, 1, // FIXME: compute mip chain
		pixels
	});
	stbi_image_free(pixels);

	return std::move(result);
}
