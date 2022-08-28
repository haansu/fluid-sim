#pragma once

#include <vector>
#include <array>

namespace Render {

	template<typename T>
	struct Vector3 {
		T x, y, z;

		//
		// DEBT | Untested methods + separation into .h .cpp for dependency avoidance
		//

		std::vector<T> ToVector() {
			return std::vector<T> { x, y, z };
		}

		std::array<T, 3> ToArray() {
			return std::array<T, 3> { x, y, z };
		}

	};

}