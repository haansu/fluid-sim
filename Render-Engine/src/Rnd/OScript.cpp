#include "pch.h"
#include "OScript.h"

#include <Rnd/ORenderer.h>

namespace rnd {

	RENDER_API OScript::OScript() {
		m_ORenderer = ORenderer::GetInstance();

		m_ORenderer->Start([&]() { return Start(); });
		m_ORenderer->Update([&]() { return Update(); });

		m_Id = ORenderer::s_IdCount;
	}

	RENDER_API OScript::~OScript() {}

}
