#pragma once
#include "pch.h"

#define ENGINE_API __declspec(dllexport)

namespace eng {
	ENGINE_API int Run() {
		return 11223;
	}
}
