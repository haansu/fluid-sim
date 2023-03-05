#pragma once

// Core
#include <Defs.h>
//

// STL
#include <vector>
#include <functional>
//

namespace Render {
	class App;
	class Winodw;
}

namespace rnd {
	
	/// <summary>
	/// Singleton Renderer class
	/// 
	/// This should be extended such that hard-coded information in the context (App class)
	/// is not necessary, but initialized here based on other input / maybe some config files.
	/// </summary>
	class ORenderer final {
	public:
		RENDER_API ~ORenderer();

		RENDER_API static ORenderer* GetInstance();

		/// <summary>
		/// Starts the Rendering Engine and passes the start and update functions.
		/// Scripts MUST be made before calling this.
		/// </summary>
		/// <returns>EXIT_CODE</returns>
		RENDER_API int Execute();
	private:
		/// <summary>
		/// Singleton
		/// </summary>
		ORenderer() = default;

		void Start(std::function<void()> start);
		void Update(std::function<void()> update);

		void CallStart();
		void CallUpdate();

		std::vector<std::function<void()>> m_StartFuncs;
		std::vector<std::function<void()>> m_UpdateFuncs;

		// Singleton instance
		static ORenderer* s_Instance;

		// Keeps count for the scripts / objects( int the future )
		static uint64_t s_IdCount;

		friend class ORenderable;
		friend class OScript;
	};

}
