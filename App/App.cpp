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
	std::cout << rnd::Run() << "\n";
	std::cout << eng::Run() << "\n";
	return 0;
}