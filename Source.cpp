

#include "FusionNames.h"

#include <iostream>

int main() {

	std::string other = "Broly";
	std::string base = "Evelyn";
	
	std::cout << "FINAL NAME = " << getFusionName(base, other) << std::endl;

	return 0;
}