#include "first_app.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <cstdlib>
#include <iostream>
#include <stdexcept>
int main() {

	lve::FirstApp app{};

	try {

		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n'; //if error output to the console
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}