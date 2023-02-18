#pragma once

// GLM
#include <glm/vec4.hpp>
//

namespace Render {
	class GColor {
	public:
		glm::vec4 rgba;
		
		static glm::vec4 White() { return glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }; }
		static glm::vec4 Black() { return glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }; }
		static glm::vec4 Red()   { return glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }; }
		static glm::vec4 Green() { return glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f }; }
		static glm::vec4 Blue()  { return glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f }; }
	};

}
