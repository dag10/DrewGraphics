#include <fstream>
#include <vector>

std::vector<char> file_contents(std::string path) {
	std::ifstream input(path, std::ios::binary);
	return std::vector<char>(
				std::istreambuf_iterator<char>(input), 
				std::istreambuf_iterator<char>());
}

