#ifndef VDE__CORE__WINDOW_IMPL_GLFW_VULKAN_H
#define VDE__CORE__WINDOW_IMPL_GLFW_VULKAN_H
#pragma once

#include <core/window.h>
#include <VkBootstrap.h>

struct GLFWwindow;

struct vde::core::Window::Impl
{
	GLFWwindow*         window;
	vkb::Instance       instance;
	VkSurfaceKHR        surface = VK_NULL_HANDLE;
	vkb::PhysicalDevice physicalDevice;
};

struct ExtensionFunctions
{
	ExtensionFunctions() = delete;

	static inline PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
	static inline PFN_vkCmdEndDebugUtilsLabelEXT   vkCmdEndDebugUtilsLabelEXT   = nullptr;
};

#endif /* VDE__CORE__WINDOW_IMPL_GLFW_VULKAN_H */
