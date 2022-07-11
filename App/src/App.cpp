#include <iostream>
#include <Engine.h>
#include <Render.h>
#include <EAPIPP.h>
#include <REAPIPP.h>
//
//	ENTRY POINT
//

int main() {
	
	std::cout << "Restructuring!\n";
	rnd::Run();
	std::cout << eng::Run() << "\n";
	return 0;
}