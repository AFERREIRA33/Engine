#ifndef IMGUI__VDE_H
#define IMGUI__VDE_H
#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

namespace vde::core
{
	class Window;
	class GraphicsContext;
}

namespace vde::imgui
{
	extern void Initialize(vde::core::Window& window, vde::core::GraphicsContext& context);
	extern void BeginFrame();
	extern void EndFrame(vde::core::GraphicsContext& context);
	extern void Shutdown();
}

#endif /* IMGUI__VDE_H */
