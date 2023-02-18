#pragma once
#include "pch.h"

//
// Hack for now
//
#define ENGINE_API __declspec(dllexport)

namespace eng {
	ENGINE_API int Run() {
		return 11223;
	}

	ENGINE_API int FK() {
		std::cout << "1234";
		return 32123;
	}
}
