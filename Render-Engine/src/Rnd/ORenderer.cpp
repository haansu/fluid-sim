#include "pch.h"
#include "ORenderer.h"

#include <Core.h>
#include <Core/App.h>

#include <vector>


NAMESPACE_START_SCOPE_RND

ORenderer* ORenderer::s_Instance = nullptr;
uint64_t ORenderer::s_IdCount = 0;

RENDER_API ORenderer* ORenderer::GetInstance() {
	if (s_Instance)
		return s_Instance;

	s_Instance = new ORenderer();
	return s_Instance;
}

RENDER_API ORenderer::~ORenderer() {}

RENDER_API int ORenderer::Execute() {
	try {
		Render::App context;
		context.Run([&]() { return CallStart(); }, [&]() { return CallUpdate(); }, [&]() -> std::unordered_map<uint64_t, ObjectSettings*> { return GetDrawList(); });
		return EXIT_SUCCESS;
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}
}

void ORenderer::Start(std::function<void()> start) {
	m_StartFuncs.push_back(start);
}

void ORenderer::Update(std::function<void()> update) {
	m_UpdateFuncs.push_back(update);
}

void ORenderer::CallStart() {
	for (const auto& func : m_StartFuncs)
		func();

	m_StartFuncs.clear();
}

void ORenderer::CallUpdate() {
	for (const auto& func : m_UpdateFuncs)
		func();
}

std::unordered_map<uint64_t, ObjectSettings*> ORenderer::GetDrawList() {
	return m_DrawListSettings;
}

NAMESPACE_END_SCOPE_RND
