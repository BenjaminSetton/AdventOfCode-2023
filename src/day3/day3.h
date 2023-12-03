// -- - Day 3: Gear Ratios-- -
// You and the Elf eventually reach a gondola lift station; he says the gondola lift will take you up to the water source, but this is as far as he can bring you.You go inside.
// 
// It doesn't take long to find the gondolas, but there seems to be a problem: they're not moving.
// 
// "Aaah!"
// 
// You turn around to see a slightly - greasy Elf with a wrench and a look of surprise. "Sorry, I wasn't expecting anyone! The gondola lift isn't working right now; it'll still be a while before I can fix it." You offer to help.
// 
// The engineer explains that an engine part seems to be missing from the engine, but nobody can figure out which one.If you can add up all the part numbers in the engine schematic, it should be easy to work out which part is missing.
// 
// The engine schematic(your puzzle input) consists of a visual representation of the engine.There are lots of numbersand symbols you don't really understand, but apparently any number adjacent to a symbol, even diagonally, is a "part number" and should be included in your sum. (Periods (.) do not count as a symbol.)
// 
// Here is an example engine schematic :
// 
// 467..114..
// ...*......
// ..35..633.
// ......#...
// 617*......
// ..... + .58.
// ..592.....
// ......755.
// ...$.*....
// .664.598..
// In this schematic, two numbers are not part numbers because they are not adjacent to a symbol : 114 (top right) and 58 (middle right).Every other number is adjacent to a symbol and so is a part number; their sum is 4361.
// 
// Of course, the actual engine schematic is much larger.What is the sum of all of the part numbers in the engine schematic ?

#include <algorithm>
#include <assert.h>
#include <unordered_map>
#include <vector>

#include "../challenge.h"

struct Day3_1 : public Challenge
{
	bool IsSymbol(char c)
	{
		return !std::isdigit(c) && (c != '.');
	}

	// Clamps x and y to remain inbounds (no wrapping), and returns the character at the index
	char SafeAt(int x, int y, Input input)
	{
		int lineSize = input[0].size();
		x = std::clamp(x, 0, lineSize);
		y = std::clamp(y, 0, (int)input.size());
		return input[y][x];
	}

	int Run(Input input)
	{
		typedef std::pair<int, int> Index;

		static const Index SEARCH_KERNEL[9] =
		{
			{ -1, -1 }, { 0, -1 }, { 1, -1 },
			{ -1, 0 }, { 0, 0 }, { 1, 0 },
			{ -1, 1 }, { 0, 1 }, { 1, 1 }
		};

		std::vector<Index> partNumberIndex;
		for (int y = 0; y < input.size(); y++)
		{
			std::string line = input[y];
			for (int x = 0; x < line.size(); x++)
			{
				char c = input[y][x];
				if (IsSymbol(c))
				{
					for (auto offset : SEARCH_KERNEL)
					{
						Index currentOffset{ x + offset.first, y + offset.second };
						char newC = SafeAt(currentOffset.first, currentOffset.second, input);
						if (std::isdigit(newC))
						{
							partNumberIndex.push_back(currentOffset);
						}
					}
				}
			}
		}

		// Look for the entire numbers
		std::unordered_map<int, std::vector<Index>> partNumbers;
		for (auto index : partNumberIndex)
		{
			std::string numberStr = "";

			// Back-track to first digit
			Index currIndex = index;
			while (currIndex.first >= 0 && std::isdigit(input[currIndex.second][currIndex.first]))
			{
				currIndex.first -= 1;
			}

			currIndex.first += 1; // Go forward one to valid index
			// Run forward to get all digits
			while (currIndex.first < input[0].size() && std::isdigit(input[currIndex.second][currIndex.first]))
			{
				numberStr += input[currIndex.second][currIndex.first];
				currIndex.first += 1;
			}

			int newNumber = std::atoi(numberStr.c_str());
			auto numberIter = partNumbers.find(newNumber);
			if (numberIter != partNumbers.end())
			{
				// Duplicate?
				// Find distance between this number and all other instances. If it's less than 3 then it's the same number, since
				// that's all the kernel traversed
				bool isDifferentNumber = true;
				auto& numberIndexList = numberIter->second;
				for (const auto& indexInstance : numberIndexList)
				{
					// Only consider horizontal distance if we're talking about the same row, otherwise it's definitely a different number
					if (indexInstance.second != index.second)
					{
						continue;
					}
					else
					{
						int horDistance = std::abs(indexInstance.first - index.first);
						if (horDistance < 3)
						{
							isDifferentNumber = false;
							break;
						}
					}
				}

				if (isDifferentNumber)
				{
					numberIndexList.push_back(index);
				}
			}
			else
			{
				// First time finding this number, add it to the list
				partNumbers.insert({ newNumber, { index } });
			}

			//if (partNumbers.find(newNumber) != partNumbers.end())
			//{
			//	std::cout << "Duplicate: " << newNumber << std::endl;
			//}
		}

		int sum = 0;
		for (const auto& iter : partNumbers)
		{
			sum += iter.first * iter.second.size();
		}

		return sum;
	}
};

// --- Part Two---
// The engineer finds the missing part and installs it in the engine!As the engine springs to life, you jump in the closest gondola, finally ready to ascend to the water source.
// 
// You don't seem to be going very fast, though. Maybe something is still wrong? Fortunately, the gondola has a phone labeled "help", so you pick it up and the engineer answers.
// 
// Before you can explain the situation, she suggests that you look out the window.There stands the engineer, holding a phone in one handand waving with the other.You're going so slowly that you haven't even left the station.You exit the gondola.
// 
// The missing part wasn't the only issue - one of the gears in the engine is wrong. A gear is any * symbol that is adjacent to exactly two part numbers. Its gear ratio is the result of multiplying those two numbers together.
// 
// This time, you need to find the gear ratio of every gearand add them all up so that the engineer can figure out which gear needs to be replaced.
// 
// Consider the same engine schematic again :
// 
// 467..114..
// ...*......
// ..35..633.
// ......#...
// 617*......
// .....+.58.
// ..592.....
// ......755.
// ...$.*....
// .664.598..
// In this schematic, there are two gears.The first is in the top left; it has part numbers 467 and 35, so its gear ratio is 16345. The second gear is in the lower right; its gear ratio is 451490. (The * adjacent to 617 is not a gear because it is only adjacent to one part number.) Adding up all of the gear ratios produces 467835.
// 
// What is the sum of all of the gear ratios in your engine schematic ?

struct Day3_2 : public Challenge
{
	// Clamps x and y to remain inbounds (no wrapping), and returns the character at the index
	char SafeAt(int x, int y, Input input)
	{
		int lineSize = input[0].size();
		x = std::clamp(x, 0, lineSize);
		y = std::clamp(y, 0, (int)input.size());
		return input[y][x];
	}

	// A gear is any '*' character with exactly two neighboring part numbers. This function
	// calculates if the given character 'c' is a gear and returns the gear ratio in that case
	// Otherwise, it returns 0
	int CalculateGearRatio(int x, int y, Input input)
	{
		typedef std::pair<int, int> Index;

		static const Index SEARCH_KERNEL[9] =
		{
			{ -1, -1 }, { 0, -1 }, { 1, -1 },
			{ -1, 0 }, { 0, 0 }, { 1, 0 },
			{ -1, 1 }, { 0, 1 }, { 1, 1 }
		};

		char c = SafeAt(x, y, input);
		if (c == '*')
		{
			std::vector<Index> neighbors;

			for (const auto& offset : SEARCH_KERNEL)
			{
				Index newIndex = { x + offset.first, y + offset.second };
				char newC = SafeAt(x + offset.first, y + offset.second, input);
				if (std::isdigit(newC))
				{
					neighbors.push_back(newIndex);
				}
			}

			// Collapse contiguous neighbors
			auto neighborsSafe = neighbors;
			Index prevNeighbor = { -1, -1 };
			int neighborsRemoved = 0;
			for (int i = 0; i < neighborsSafe.size(); i++)
			{
				Index neighbor = neighborsSafe[i];
				if (neighbor.second == prevNeighbor.second)
				{
					// The current neighbor is right next to the previous neighbor horizontally (let's remove the previous neighbor)
					if (neighbor.first - 1 == prevNeighbor.first)
					{
						neighbors.erase(neighbors.begin() + (i - neighborsRemoved));
						neighborsRemoved++;
					}
				}

				prevNeighbor = neighbor;
			}

			// Get the actual numbers. These don't matter unless we have exactly two
			std::vector<int> gearNumbers;
			if (neighbors.size() == 2)
			{
				for (const auto& index : neighbors)
				{
					Index currIndex = index;
					std::string numberStr = "";
					while (currIndex.first >= 0 && std::isdigit(input[currIndex.second][currIndex.first]))
					{
						currIndex.first -= 1;
					}

					currIndex.first += 1; // Go forward one to valid index
					// Run forward to get all digits
					while (currIndex.first < input[0].size() && std::isdigit(input[currIndex.second][currIndex.first]))
					{
						numberStr += input[currIndex.second][currIndex.first];
						currIndex.first += 1;
					}

					gearNumbers.push_back(std::atoi(numberStr.c_str()));
				}

				assert(gearNumbers.size() == 2);

				return gearNumbers[0] * gearNumbers[1];
			}
		}

		return 0;
	}

	int Run(Input input)
	{
		int sum = 0;
		for (int y = 0; y < input.size(); y++)
		{
			auto line = input[y];
			for (int x = 0; x < line.size(); x++)
			{
				sum += CalculateGearRatio(x, y, input);
			}
		}

		return sum;
	}
};