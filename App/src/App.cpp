//	Title: Waterfall
//	By: Ioan-Lica Marin
//	@2023

//
//	ENTRY POINT
//

#include <iostream>
#include <Engine.h>
#include <Rnd/ORenderer.h>

#include "Simulation.h"

int main() {
	std::cout << "Version - a0.1\n";

	// The rnd::OScript constructor sets it up to be ran inside the renderer
	Simulation mainScript;

	// Renderer is created and execution is started
	rnd::ORenderer* renderer = rnd::ORenderer::GetInstance();
	int Err = renderer->Execute();

	return Err;
}