#pragma once

#include "defs.h"

NAMESPACE_START_SCOPE_RND

class UIHelper {
public:
	RENDER_API static void ReadSimulationStartParams(int& xSpeed, int& ySpeed, int& zSpeed, int& particleCount);
	RENDER_API static void ReadSimulationData(bool& reset, bool& gravity, bool& collisions, double& viscosity, double& restDesnity, double& damping, double& stiffness);
};

NAMESPACE_END_SCOPE_RND