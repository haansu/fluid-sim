#pragma once

#include <Defs.h>
// GLM
#include <glm/mat4x4.hpp>
//

namespace Render {

	// Forward declare
	enum class ProjectionMethodType;

	class GCamera {
	public:
		void SetOrthoProjection(float left, float right, float top, float bottom, float near, float far);
		void SetPerspProjection(float yFOV, float aspectRatio, float near, float far);

		ProjectionMethodType GetProjectionMethodType();
		bool IsPerspective();

		const glm::mat4& GetProjectionMatrix();

	private:
		glm::mat4 m_ProjectionMatrix;
		ProjectionMethodType m_ProjectionMethodType;
	};

}
