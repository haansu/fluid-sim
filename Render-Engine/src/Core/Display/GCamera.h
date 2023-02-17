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

		void SetViewDir(glm::vec3 pos, glm::vec3 dir);
		void SetViewTrg(glm::vec3 pos, glm::vec3 trg);
		void SetViewVec(glm::vec3 pos, glm::vec3 rot);

		const glm::mat4& GetProjMat() { return m_ProjMat; }
		const glm::mat4& GetViewMat() { return m_ViewMat; }
		const glm::mat4& GetInvViewMat() { return m_InvViewMat; }
		const glm::vec3 GetPosVec() { return glm::vec3(m_InvViewMat[3]); }

	private:
		glm::vec3 m_Up = glm::vec3(0.0f, 0.0f, 1.0f);

		glm::mat4 m_ProjMat{ 1.0f };
		glm::mat4 m_ViewMat{ 1.0f };
		glm::mat4 m_InvViewMat{ 1.0f };

		ProjectionMethodType m_ProjectionMethodType;
	};

}
