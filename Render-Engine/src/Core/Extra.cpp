#include "pch.h"
#include "Extra.h"

// Core
#include <Core.h>
//

// STL
#include <vector>
//

namespace Render {

	VkResult CreateDebugUtilsMesageEXT(
		  VkInstance instance
		, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
		, const VkAllocationCallbacks* pAllocator
		, VkDebugUtilsMessengerEXT* pDebugMessenger) {

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance
		, VkDebugUtilsMessengerEXT debugMessenger
		, const VkAllocationCallbacks* pAllocator) {

	}

	glm::mat4 Trf::Model() {
		const float rcosx = glm::cos(rotate.x);
		const float rsinx = glm::sin(rotate.x);
		const float rcosy = glm::cos(rotate.y);
		const float rsiny = glm::sin(rotate.y);
		const float rcosz = glm::cos(rotate.z);
		const float rsinz = glm::sin(rotate.z);

		return glm::mat4{{
				  scale.x * (rcosy * rcosz + rsiny * rsinx * rsinz)
				, scale.x * (rcosx * rsinz)
				, scale.x * (rcosy * rsinx * rsinz - rcosz * rsiny)
				, 0.0f
			}, {
				  scale.y * (rcosz * rsiny * rsinx - rcosy * rsinz)
				, scale.y * (rcosx * rcosz)
				, scale.y * (rcosy * rcosz * rsinx + rsiny * rsinz)
				, 0.0f
			}, {
				  scale.z * (rcosx * rsiny)
				, scale.z * (-rsinx)
				, scale.z * (rcosy * rcosx)
				, 0.0f
			}, {
				  translate.x
				, translate.y
				, translate.z
				, 1.0f
			}
		};
	}

	glm::mat3 Trf::Normal() {
		const float cosx = glm::cos(rotate.x);
		const float sinx = glm::sin(rotate.x);
		const float cosy = glm::cos(rotate.y);
		const float siny = glm::sin(rotate.y);
		const float cosz = glm::cos(rotate.z);
		const float sinz = glm::sin(rotate.z);

		const glm::vec3 scaleInv = 1.0f / scale;

		return glm::mat3{ {
				  scaleInv.x * (cosy * cosz + siny * sinx * sinz)
				, scaleInv.x * (cosx * sinz)
				, scaleInv.x * (cosy * sinx * sinz - cosz * siny)
			}, {
				  scaleInv.y * (cosz * siny * sinx - cosy * sinz)
				, scaleInv.y * (cosx * cosz)
				, scaleInv.y * (cosy * cosz * sinx + siny * sinz)
			}, {
				  scaleInv.z * (cosx * siny)
				, scaleInv.z * (-sinx)
				, scaleInv.z * (cosy * cosx)
			},
		};
	}

}