#include "pch.h"
#include "OScript.h"

#include <Rnd/ORenderer.h>

NAMESPACE_START_SCOPE_RND

RENDER_API OScript::OScript() {
	m_ORenderer = ORenderer::GetInstance();

	m_ORenderer->Start([&]() { return Start(); });
	m_ORenderer->Update([&]() { return Update(); });

	m_Id = ORenderer::s_IdCount;
}

RENDER_API OScript::~OScript() {}

NAMESPACE_END_SCOPE_RND
