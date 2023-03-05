#pragma once

#include <Defs.h>

namespace rnd {

	// Forward declare
	class ORenderer;

	/// <summary>
	/// An object that inherits from OScript will have the Start() and Update() methods
	/// automatically called in the propper states of the render engine, i.e. Start() will
	/// be called when the render engine is initialized and Update() will be called in the
	/// main loop.
	/// 
	/// This will probably change as it's a better idea to have scripts attached to objects,
	/// so that the script will have the same lifetime as the object it's being attached to.
	/// </summary>
	class OScript {
	public:
		RENDER_API OScript();
		RENDER_API ~OScript();
	protected:

		virtual void Start() = 0;
		virtual void Update() = 0;

	private:

		ORenderer* m_ORenderer;
		uint64_t m_Id;
	};

}
