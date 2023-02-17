#pragma once

namespace Render {

	class Time {
	public:
		static float DeltaTime() { return s_DeltaTime; }
	private:
		static void _SetDeltaTime(const float& dt) { s_DeltaTime = dt; }
	private:
		static float s_DeltaTime;
		friend class App;
	};

}
