#include "Utils.h"
#include <fstream>

std::string loadAsciiFile(const char* filename) {
	std::ifstream file;
	file.open(filename);
	std::string str((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	file.close();
	return str;
}
