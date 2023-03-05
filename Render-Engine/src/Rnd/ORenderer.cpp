#include "pch.h"
#include "ORenderer.h"

#include <Core/App.h>
#include <Core.h>

namespace rnd {

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
			context.Run([&]() { return CallStart(); }, [&]() { return CallUpdate(); });
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
	}

	void ORenderer::CallUpdate() {
		for (const auto& func : m_UpdateFuncs)
			func();
	}
}