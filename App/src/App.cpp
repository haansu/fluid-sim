#include <iostream>
#include <Engine.h>
#include <Core/Render.h>

//
//	ENTRY POINT
//

int main() {
	
	std::cout << "Version - a0.1\n";

	try {
		rnd::Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}