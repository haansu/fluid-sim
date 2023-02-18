#pragma once

// Core
#include "App.h"
#include <Core.h>
#include <Defs.h>
#include <Window/Window.h>
//

namespace Render {

    RENDER_API void Run() {
        App core;
        core.Run();
    }
}
