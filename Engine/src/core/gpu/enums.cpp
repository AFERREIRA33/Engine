#include <core/gpu/enums.h>

vde::core::gpu::EImageUsageBits operator|(vde::core::gpu::EImageUsageBits a, vde::core::gpu::EImageUsageBits b)
{
	return vde::core::gpu::EImageUsageBits(uint32_t(a) | uint32_t(b));
}

vde::core::gpu::EImageUsageBits operator&(vde::core::gpu::EImageUsageBits a, vde::core::gpu::EImageUsageBits b)
{
	return vde::core::gpu::EImageUsageBits(uint32_t(a) & uint32_t(b));
}

vde::core::gpu::EBufferUsageBits operator|(vde::core::gpu::EBufferUsageBits a, vde::core::gpu::EBufferUsageBits b)
{
	return vde::core::gpu::EBufferUsageBits(uint32_t(a) | uint32_t(b));
}

vde::core::gpu::EBufferUsageBits operator&(vde::core::gpu::EBufferUsageBits a, vde::core::gpu::EBufferUsageBits b)
{
	return vde::core::gpu::EBufferUsageBits(uint32_t(a) & uint32_t(b));
}
