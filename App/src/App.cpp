#include <iostream>
#include <Engine.h>
#include <Rnd/ORenderer.h>
#include <Rnd/OScript.h>

//
//	ENTRY POINT
//

int main() {
	std::cout << "Version - a0.1\n";

	rnd::ORenderer* renderer = rnd::ORenderer::GetInstance();
	int Err = renderer->Execute();
	
	return Err;
}