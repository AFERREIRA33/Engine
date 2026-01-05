#version 460
//! #extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iColor;

layout(location = 0) out vec3 vColor;

layout(push_constant) uniform constants
{
	mat4 view;
	mat4 projection;
} PushConstants;

void main()
{
	gl_Position = PushConstants.projection * PushConstants.view * vec4(iPosition, 1.0f);
	vColor = iColor;
}
