#version 460

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexturePosition;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D textureSampler;

void main() {
	outColor = texture(textureSampler, fragTexturePosition);
}