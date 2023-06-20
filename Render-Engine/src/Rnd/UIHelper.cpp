#include "pch.h"
#include "UIHelper.h"

#include "../Core/UI/UI.h"

NAMESPACE_START_SCOPE_RND

RENDER_API void UIHelper::ReadSimulationStartParams(int& xSpeed, int& ySpeed, int& zSpeed, int& particleCount) {
	xSpeed = Render::UI::xSpeed;
	ySpeed = Render::UI::ySpeed;
	zSpeed = Render::UI::zSpeed;
	particleCount = Render::UI::particleCount;
}

RENDER_API void UIHelper::ReadSimulationData(bool& reset, bool& gravity, bool& collisions, double& viscosity, double& restDesnity, double& damping, double& stiffness) {
	reset = Render::UI::bReset;
	gravity = Render::UI::bGravity;
	collisions = Render::UI::bCollisions;
	viscosity = static_cast<double>(Render::UI::viscosity);
	restDesnity = static_cast<double>(Render::UI::restDesnity);
	damping = static_cast<double>(Render::UI::damping);
	stiffness = static_cast<double>(Render::UI::stiffness);
}

NAMESPACE_END_SCOPE_RND