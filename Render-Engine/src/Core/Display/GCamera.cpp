#include "pch.h"
#include "GCamera.h"
#include "GCameraEnums.h"

// Core
#include <Core.h>
//

namespace Render {

	void GCamera::SetOrthoProjection(float left, float right, float top, float bottom, float near, float far) {
		m_ProjMat = glm::mat4{ 1.0f };
		m_ProjMat[0][0] = 2.0f / (right - left);
		m_ProjMat[1][1] = 2.0f / (bottom - top);
		m_ProjMat[2][2] = 1.0f / (far - near);
		m_ProjMat[3][0] = -(right + left) / (right - left);
		m_ProjMat[3][1] = -(bottom + top) / (bottom - top);
		m_ProjMat[3][2] = -near / (far - near);
	}

	void GCamera::SetPerspProjection(float yFOV, float aspectRatio, float near, float far) {
		assert(glm::abs(aspectRatio - std::numeric_limits<float>::epsilon()) > 0.0f);

		const float halfFOVTg = tan(yFOV / 20.f);

		m_ProjMat = glm::mat4{ 0.0f };
		m_ProjMat[0][0] = 1.0f / (aspectRatio * halfFOVTg);
		m_ProjMat[1][1] = 1.0f / (halfFOVTg);
		m_ProjMat[2][2] = far / (far - near);
		m_ProjMat[2][3] = 1.0f;
		m_ProjMat[3][2] = -(far * near) / (far - near);
	}

	void GCamera::SetViewDir(glm::vec3 pos, glm::vec3 dir, glm::vec3 up) {
		const glm::vec3 w{ glm::normalize(dir) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		m_ViewMat = glm::mat4{ 1.0f };
		m_ViewMat[0][0] = u.x;
		m_ViewMat[1][0] = u.y;
		m_ViewMat[2][0] = u.z;
		m_ViewMat[0][1] = v.x;
		m_ViewMat[1][1] = v.y;
		m_ViewMat[2][1] = v.z;
		m_ViewMat[0][2] = w.x;
		m_ViewMat[1][2] = w.y;
		m_ViewMat[2][2] = w.z;
		m_ViewMat[3][0] = -glm::dot(u, pos);
		m_ViewMat[3][1] = -glm::dot(v, pos);
		m_ViewMat[3][2] = -glm::dot(w, pos);

		m_InvViewMat = glm::mat4{ 1.f };
		m_InvViewMat[0][0] = u.x;
		m_InvViewMat[0][1] = u.y;
		m_InvViewMat[0][2] = u.z;
		m_InvViewMat[1][0] = v.x;
		m_InvViewMat[1][1] = v.y;
		m_InvViewMat[1][2] = v.z;
		m_InvViewMat[2][0] = w.x;
		m_InvViewMat[2][1] = w.y;
		m_InvViewMat[2][2] = w.z;
		m_InvViewMat[3][0] = pos.x;
		m_InvViewMat[3][1] = pos.y;
		m_InvViewMat[3][2] = pos.z;
	}

	void GCamera::SetViewTrg(glm::vec3 pos, glm::vec3 trg) {
		SetViewDir(pos, trg - pos);
	}

	void GCamera::SetViewVec(glm::vec3 pos, glm::vec3 rot) {
		const float cosx = glm::cos(rot.x);
		const float sinx = glm::sin(rot.x);
		const float cosy = glm::cos(rot.y);
		const float siny = glm::sin(rot.y);
		const float cosz = glm::cos(rot.z);
		const float sinz = glm::sin(rot.z);

		const glm::vec3 u {
			  (cosy * cosz + siny * sinx * sinz)
			, (cosx * sinz)
			, (cosy * sinx * sinz - cosz * siny)
		};

		const glm::vec3 v {
			  (cosz * siny * sinx - cosy * sinz)
			, (cosx * cosz)
			, (cosy * cosz * sinx + siny * sinz)
		};

		const glm::vec3 w {
			  (cosx * siny)
			, (-sinx)
			, (cosy * cosx)
		};

		m_ViewMat = glm::mat4{ 1.f };
		m_ViewMat[0][0] = u.x;
		m_ViewMat[1][0] = u.y;
		m_ViewMat[2][0] = u.z;
		m_ViewMat[0][1] = v.x;
		m_ViewMat[1][1] = v.y;
		m_ViewMat[2][1] = v.z;
		m_ViewMat[0][2] = w.x;
		m_ViewMat[1][2] = w.y;
		m_ViewMat[2][2] = w.z;
		m_ViewMat[3][0] = -glm::dot(u, pos);
		m_ViewMat[3][1] = -glm::dot(v, pos);
		m_ViewMat[3][2] = -glm::dot(w, pos);

		m_InvViewMat = glm::mat4{ 1.f };
		m_InvViewMat[0][0] = u.x;
		m_InvViewMat[0][1] = u.y;
		m_InvViewMat[0][2] = u.z;
		m_InvViewMat[1][0] = v.x;
		m_InvViewMat[1][1] = v.y;
		m_InvViewMat[1][2] = v.z;
		m_InvViewMat[2][0] = w.x;
		m_InvViewMat[2][1] = w.y;
		m_InvViewMat[2][2] = w.z;
		m_InvViewMat[3][0] = pos.x;
		m_InvViewMat[3][1] = pos.y;
		m_InvViewMat[3][2] = pos.z;
	}
	void GCamera::SetViewVec(glm::vec3 right, glm::vec3 up, glm::vec3 forward) {
		m_ViewMat[0] = glm::vec4(right, 0.0f);
		m_ViewMat[1] = glm::vec4(up, 0.0f);
		m_ViewMat[2] = glm::vec4(forward, 0.0f);
	}

	
	ProjectionMethodType GCamera::GetProjectionMethodType() { return m_ProjectionMethodType; }
	
	bool GCamera::IsPerspective() {
		return (m_ProjectionMethodType == ProjectionMethodType::Perspective) ? true : false;
	}

}
