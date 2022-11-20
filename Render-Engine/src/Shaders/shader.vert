#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexturePosition;

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
} uniBuff;

void main() {
	gl_Position = uniBuff.proj * uniBuff.view * uniBuff.model * vec4(inPosition, 1.0);
	fragColor = inColor;
	fragUV = inUV;
}