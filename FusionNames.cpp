#include "FusionNames.h"

#include <ctype.h>

#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <set>

static const float BASE_NAME_PERCENT = 0.70f;
static const float OTHER_NAME_PERCENT = 0.30f;

static const float SECONDARY_BASE_NAME_PERCENT = 0.30f;
static const float SECONDARY_OTHER_NAME_PERCENT = 0.80f;

static const int MIN_DECREASE_COUNT = 5; // The minimum name length required to use less characters from the name than the original length

// anonymous namespace for private functions declarations
namespace {
	// If a substring of subname appears multiple times consecutively in original, remove it unless it also exists in one of the two original names multiple times
	// Return original with duplicates removed
	std::string eraseSubstringDuplicates(std::string original, std::string subname, std::string base, std::string other);

	// Fix any problems with the input name
	// Names separated with spaces will use the longest subname (ex: F FF A --> return FF)
	// Names with both characters and non-characters will have any non-characters removed
	// Return the corrected name
	std::string fixName(std::string name);

	// Combine other and base
	// secondary is set to true if the secondary name percentages will be used instead of the normal ones
	// return a combination of the two input names
	std::string combine(std::string other, std::string base, bool secondary);
}

namespace FusionNames
{

	bool isCharNonLetter(char c) {
		char lower = tolower(c);
		return toupper(lower) == lower;
	}

	std::string getFusionName(std::string base, std::string other) {

		if (other == base)
			return other;

		std::string finalName;
		std::string preferredMerge = combine(other, base, false);
		std::string reverseMerge = combine(base, other, false);

		//std::cout << preferredMerge << std::endl;
		//std::cout << reverseMerge << std::endl;

		// This will be set to true if ((preferredMerge && reverseMerge) == (other || base)) and other != base
		// Redo the merge using the secondary name percentages if this is true
		bool redoMerge = false;

		if (preferredMerge == other || preferredMerge == base) {
			if (reverseMerge != base && reverseMerge != other) {
				finalName = reverseMerge;
			}
			else {
				redoMerge = true;
			}
		}
		else if (reverseMerge == base || reverseMerge == other) {
			if (preferredMerge != other && preferredMerge != base) {
				finalName = preferredMerge;
			}
			else {
				redoMerge = true;
			}
		}
		else {
			finalName = preferredMerge;
		}

		if (redoMerge) {
			finalName = combine(other, base, true);
		}

		return finalName;
	}

}
// anonymous namespace for private functions definitions
namespace {
	std::string eraseSubstringDuplicates(std::string original, std::string subname, std::string base, std::string other) {

		// Create all combinations of substrings of subname
		std::set<std::string> substrings;
		for (size_t o = 0; o < subname.size(); o++) {
			for (size_t i = 0; i < subname.size(); i++) {
				std::string substring = subname.substr(i, i + o);
				substrings.insert(substring);
			}
		}

		substrings.insert(subname);

		// Check if each substring is repeated
		for (std::string s : substrings) {

			size_t first = original.find(s);
			// s exists atleast once
			if (first != std::string::npos) {

				size_t second = original.find(s, first + s.size());
				// s exists atleast twice, and the second appearance is immediately after the first
				if (second != std::string::npos && second == first + s.size()) {

					std::string repeatedSequence = s + s;

					// If this sequence does not appear in any of the two initial names, then remove the duplicate
					if (other.find(repeatedSequence) == std::string::npos && base.find(repeatedSequence) == std::string::npos) {

						// std::cout << "Removed Duplicate. Original = " << original << std::endl;
						original.erase(second, s.size());
						break;
					}
				}
			}
		}

		return original;
	}

	std::string fixName(std::string name) {

		std::string finalName;

		// If there are spaces in the name, return the subname with the most characters in it
		std::stringstream ss(name);
		std::istream_iterator<std::string> begin(ss);
		std::istream_iterator<std::string> end;
		std::vector<std::string> subnames(begin, end);

		size_t longest = 0;

		// Determine the index of the longest subname
		for (size_t i = 1; i < subnames.size(); i++) {
			if (subnames[i].length() > subnames[longest].length())
				longest = i;
		}

		finalName = subnames[longest];

		// Use this to determine if the name does not have any letters in it
		std::string lowerName, upperName;
		for (char c : finalName) {
			lowerName += tolower(c);
			upperName += toupper(c);
		}

		// All of the characters are the same regardless of case --> The name consists of only non-letters
		// Return the name without modifying it
		if (lowerName == upperName)
			return finalName;

		// Remove all non-letter characters
		finalName.erase(std::remove_if(finalName.begin(), finalName.end(), FusionNames::isCharNonLetter), finalName.end());

		return finalName;
	}


	std::string combine(std::string other, std::string base, bool secondary) {

		other = fixName(other);
		base = fixName(base);

		std::string name;
		int count = 0, maxCount = ceil(base.size() * (secondary ? SECONDARY_BASE_NAME_PERCENT : BASE_NAME_PERCENT));

		// Prevent the maxCount from being larger than entire name size, and prevent entire name being used if the name is short
		if (maxCount >= base.size() && maxCount >= MIN_DECREASE_COUNT)
			maxCount -= 1;

		// The start of the name consists of 70% of the base name
		for (char c : base) {
			if (count++ >= maxCount)
				break;
			name += c;
		}

		// The end of the name consists of the last 30% of the other name
		maxCount = ceil(other.size() * (secondary ? SECONDARY_OTHER_NAME_PERCENT : OTHER_NAME_PERCENT));

		// If the other name length is atleast 1.5 times the length of the base name, use an extra character
		if (float(other.size()) / base.size() >= 1.5) {
			maxCount += 1;
		}

		while (maxCount >= other.size() && maxCount >= MIN_DECREASE_COUNT)
			maxCount -= 1;

		std::string secondName = "";
		for (size_t i = other.size() - maxCount; i < other.size(); i++) {
			name += other[i];
			secondName += other[i];
		}

		// std::cout << secondName << std::endl;

		name = eraseSubstringDuplicates(name, secondName, base, other);

		return name;

	}

}
