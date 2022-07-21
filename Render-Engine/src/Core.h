#pragma once

#define GLFW_INCLUDE_VULKAN

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.h>

//
// Adapt for future logging system
#ifdef DEBUG
constexpr bool DEBUG_MESSGE = true;
#else
constexpr bool DEBUG_MESSGE = false;
#endif // DEBUG