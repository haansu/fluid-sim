#pragma once

#include <vector>
#include <array>

namespace rnd {

	template <typename T>
	struct Vector3 {
		T x, y;

		//
		// DEBT | Untested methods + separation into .h .cpp for dependency avoidance
		//

		std::vector<T> ToVector() {
			return std::vector<T> { x, y };
		}

		std::array<T, 2> ToArray() {
			return std::array<T, 2> { x, y };
		}

	};

}