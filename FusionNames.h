#pragma once

#include <string>

namespace FusionNames {

	// Determine if the specified character is a letter or not.
	// If the uppercase version of the char is the same as the lowercase version, it is not a letter
	bool isCharNonLetter(char c);

	// Return a fusion name based on other and base
	std::string getFusionName(std::string base, std::string other);
}
