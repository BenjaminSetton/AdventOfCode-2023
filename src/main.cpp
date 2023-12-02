
#include <fstream>
#include <iostream>

#include "day2/day2.h"

static const std::string inputFilePath = "../../src/day2/input2_2.txt";

int main()
{
	static constexpr uint32_t MAX_BUFFER_SIZE = 250;
	char buffer[MAX_BUFFER_SIZE];
	std::vector<std::string> input;

	std::ifstream fileHandle(inputFilePath.c_str(), std::ios::in);
	if (!fileHandle.good())
	{
		std::cout << "[ERROR] Failed to open input file '" << inputFilePath.c_str() << "'!" << std::endl;
		return -1;
	}

	while (fileHandle.good())
	{
		fileHandle.getline(buffer, MAX_BUFFER_SIZE);
		input.push_back(buffer);
	}

	Day2_2 challenge;
	std::cout << "Output: " << challenge.Run(input) << std::endl;

	fileHandle.close();
}