#include "FusionNames.h"

#include <iostream>

int main() {

	std::string other, base;

	std::cout << "Enter Name 1: ";
	std::cin >> other;
	std::cout << "Enter Name 2: ";
	std::cin >> base;
	
	std::cout << "FINAL NAME = " << FusionNames::getFusionName(base, other) << std::endl;

	return 0;
}