#pragma once
#include "pch.h"
#include "Defs.h"

namespace eng {
	ENGINE_API int Run() {
		return 11223;
	}

	ENGINE_API int FK() {
		std::cout << "1234";
		return 32123;
	}
}
