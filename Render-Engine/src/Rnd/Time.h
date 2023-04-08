#pragma once

#include <Defs.h>

// Forward declaring
namespace Render {
	class App;
}

NAMESPACE_START_SCOPE_RND

class Time {
public:
	RENDER_API static float DeltaTime() { return s_DeltaTime; }
private:
	static void _SetDeltaTime(float dt) { s_DeltaTime = dt; }

	static float s_DeltaTime;
	friend class Render::App;
};

NAMESPACE_END_SCOPE_RND
