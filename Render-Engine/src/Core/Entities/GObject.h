#pragma once

#include "../../Defs.h"
#include <memory>

// GLM
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
//

namespace Render {

	struct Transform3D {
		glm::vec3 translate;
		glm::mat3 mat() { return glm::mat3{ 1.0f }; };
	};

	class GObject {
	public:
		GObject(const GObject&) = delete;
		GObject& operator=(const GObject&) = delete;
		GObject(GObject&&) = default;
		GObject& operator=(GObject&&) = default;

		inline static GObject CreateGObject() {
			static uint32_t currentID = 0;
			return GObject{ currentID++ };
		}

		inline uint32_t GetID() { return m_ID; }

		Transform3D transfrom;
	private:
		GObject(uint32_t id)
			: m_ID(id) {};

		uint32_t m_ID;
	};

}
