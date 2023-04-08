#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

// layout(location = 4) in vec3 inLightDirection;
// layout(location = 5) in vec3 inAmbient;
// layout(location = 5) in vec3 inDiffuse;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUV;

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	vec4 lightDir;
	vec4 lightColor;
	vec4 lightAmbient;
	vec4 lightDiffuse;
} uniBuff;

layout(push_constant) uniform Push {
  mat4 modelMat;
  mat4 normalMat;
  vec4 color;
} push;

void main() {
	vec4 posWorld = push.modelMat * vec4(inPosition, 1.0);
	gl_Position = uniBuff.proj * uniBuff.view * posWorld;
	
	vec4 normalWS = vec4(normalize(mat3(push.modelMat) * inNormal), 0);

	float lightIntensity = max(dot(normalWS, uniBuff.lightDir), 0);
	
	vec4 finalIntensity = lightIntensity * uniBuff.lightDiffuse + uniBuff.lightAmbient;

	fragColor = finalIntensity * push.color;
	fragUV = inUV;
}
