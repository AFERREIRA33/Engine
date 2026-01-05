#version 460
//! #extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 vColor;

layout(location = 0) out vec4 oFragColor;

void main()
{
	oFragColor = vec4(vColor, 1.0);
}
