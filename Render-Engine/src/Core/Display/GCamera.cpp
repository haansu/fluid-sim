#include "pch.h"
#include "GCamera.h"
#include "GCameraEnums.h"

namespace Render {

	void GCamera::SetOrthoProjection(float left, float right, float top, float bottom, float near, float far) {
		m_ProjectionMatrix = glm::mat4{ 1.0f };
		m_ProjectionMatrix[0][0] = 2.0f / (right - left);
		m_ProjectionMatrix[1][1] = 2.0f / (bottom - top);
		m_ProjectionMatrix[2][2] = 1.0f / (far - near);
		m_ProjectionMatrix[3][0] = -(right + left) / (right - left);
		m_ProjectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		m_ProjectionMatrix[3][2] = -near / (far - near);
	}

	void GCamera::SetPerspProjection(float yFOV, float aspectRatio, float near, float far) {
		// TO DO: Check for aspect ration
		m_ProjectionMatrix = glm::mat4{ 1.0f };
		m_ProjectionMatrix[0][0] = 1.0f / (aspectRatio - tan(yFOV / 2.0f));
		m_ProjectionMatrix[1][1] = 1.0f / (tan(yFOV / 2.0f));
		m_ProjectionMatrix[2][2] = far / (far - near);
		m_ProjectionMatrix[2][3] = 1.0f;
		m_ProjectionMatrix[3][2] = -(far * near) / (far - near);
	}

	ProjectionMethodType GCamera::GetProjectionMethodType() { return m_ProjectionMethodType; }
	bool GCamera::IsPerspective() {
		return (m_ProjectionMethodType == ProjectionMethodType::Perspective) ? true : false;
	}

	const glm::mat4& GCamera::GetProjectionMatrix() { return m_ProjectionMatrix; }

}
