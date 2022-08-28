#pragma once

#include "Defs.h"
#include "Core.h"
#include "Window/Window.h"
#include "App.h"

namespace Render {

    RENDER_API void Run() {
        App core;
        core.Run();
    }
}
