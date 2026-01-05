#include "window_impl_glfw_vulkan.h"
#include <GLFW/glfw3.h>

#ifndef NDEBUG
# pragma comment(lib, "VkBootstrap-d.lib")
# pragma comment(lib, "glfw3-s-d.lib")
#else // !NDEBUG
# pragma comment(lib, "VkBootstrap.lib")
# pragma comment(lib, "glfw3-s.lib")
#endif // !NDEBUG

#pragma comment(lib, "vulkan-1.lib")

using namespace vde::core;

Window::Window(const WindowDescriptor& desc)
	: m_pImpl(new Impl)
	, m_desc(desc)
{
	m_pImpl->instance = vkb::InstanceBuilder{}.set_app_name(desc.title.c_str())
	                                          .set_engine_name("LNX VDE")
	                                          .request_validation_layers()
	                                          .enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
	                                          .use_default_debug_messenger()
		                                      .require_api_version(1, 3, 0)
	                                          .build().value();

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	m_pImpl->window = glfwCreateWindow(desc.size.x, desc.size.y, desc.title.c_str(), nullptr, nullptr);
	glfwCreateWindowSurface(m_pImpl->instance, m_pImpl->window, nullptr, &m_pImpl->surface);

	VkPhysicalDeviceFeatures features {};
	features.samplerAnisotropy = true;
	features.geometryShader    = true;
	features.multiDrawIndirect = true;

	VkPhysicalDeviceVulkan12Features features12 { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress       = true;
	features12.descriptorIndexing        = true;
	features12.timelineSemaphore         = true;
	features12.shaderOutputViewportIndex = true;
	features12.shaderOutputLayer         = true;

	VkPhysicalDeviceVulkan13Features features13 { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features13.dynamicRendering = true;
	features13.synchronization2 = true;

	VkPhysicalDeviceRobustness2FeaturesEXT robustness2Features { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT };
	robustness2Features.nullDescriptor = true;

	m_pImpl->physicalDevice = vkb::PhysicalDeviceSelector{ m_pImpl->instance }.set_surface(m_pImpl->surface)
		                                                                      .set_minimum_version(1, 3)
		                                                                      .set_required_features(features)
		                                                                      .set_required_features_12(features12)
		                                                                      .set_required_features_13(features13)
		                                                                      .add_required_extension(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME)
	                                                                          .add_required_extension(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME)
		                                                                      .add_required_extension_features(robustness2Features)
		                                                                      .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
																			  .allow_any_gpu_device_type(false)
		                                                                      .select().value();

#ifndef NDEBUG
	ExtensionFunctions::vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_pImpl->instance, "vkCmdBeginDebugUtilsLabelEXT");
	ExtensionFunctions::vkCmdEndDebugUtilsLabelEXT   = (PFN_vkCmdEndDebugUtilsLabelEXT  )vkGetInstanceProcAddr(m_pImpl->instance, "vkCmdEndDebugUtilsLabelEXT");
#endif /* !NDEBUG */
}

Window::~Window() noexcept
{
	vkDestroySurfaceKHR(m_pImpl->instance, m_pImpl->surface, nullptr);
	vkb::destroy_instance(m_pImpl->instance);
	glfwDestroyWindow(m_pImpl->window);
	glfwTerminate();
}

const WindowDescriptor& Window::GetDesc() const
{
	return m_desc;
}

Window::Impl& Window::GetImpl()
{
	return *m_pImpl;
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_pImpl->window);
}

void Window::RaiseShouldClose()
{
	glfwSetWindowShouldClose(m_pImpl->window, GLFW_TRUE);
}

void Window::PollEvents()
{
	glfwPollEvents();
}
