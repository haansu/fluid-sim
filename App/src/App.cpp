//	Title: Waterfall
//	By: Ioan-Lica Marin
//	@2024

//
//	ENTRY POINT
//

#include <iostream>
#include <Engine.h>
#include <Rnd/ORenderer.h>
#include <Rnd/OScript.h>
#include <Rnd/Entity.h>
#include <Rnd/Time.h>

#include <array>
#include <chrono>
#include <random>
#include <set>

#include <glm/glm.hpp>

class MainScript : rnd::OScript {
private:

	// Runs when script gets initialized inside the renderer
	void Start() {
		
	}

	// Runs on every frame
	void Update() {
		
	}

};

int main() {
	std::cout << "Version - a0.1\n";

	// The rnd::OScript constructor sets it up to be ran inside the renderer
	MainScript mainScript;
	
	// Renderer is created and execution is started
	rnd::ORenderer* renderer = rnd::ORenderer::GetInstance();
	int Err = renderer->Execute();
	
	return Err;
}