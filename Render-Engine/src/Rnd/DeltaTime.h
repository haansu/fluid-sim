#pragma once

#include <Defs.h>
#include "Time.h"

NAMESPACE_START_SCOPE_RND

class DeltaTime {
public:
	RENDER_API static float Get() { return Time::DeltaTime(); }
};

NAMESPACE_END_SCOPE_RND
