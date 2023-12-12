
#include "../challenge.h"

#include <algorithm>
#include <unordered_map>
#include <utility>

struct Day11_1 : public Challenge
{
	void PrintInput(Input& input)
	{
		std::cout << "\n\n";
		for (const auto& line : input)
		{
			std::cout << line.c_str() << std::endl;
		}
		std::cout << "\n\n";
	}

	void BresenhamLow(std::pair<int, int> p1, std::pair<int, int> p2, int& steps, Input& input)
	{
		int dx = p2.first - p1.first;
		int dy = p2.second - p1.second;
		int yi = 1;
		if (dy < 0)
		{
			yi = -1;
			dy = -dy;
		}

		int D = (2 * dy) - dx;
		int y = p1.second;
		for (int x = p1.first; x < p2.first; x++)
		{
			if (D > 0)
			{
				y = y + yi;
				D = D + (2 * (dy - dx));
				steps += 2; // Can't take diagonal steps
			}
			else
			{
				D = D + 2 * dy;
				steps++;
			}
			input[y][x] = 'O';
		}
	}

	void BresenhamHigh(std::pair<int, int> p1, std::pair<int, int> p2, int& steps, Input& input)
	{
		int dx = p2.first - p1.first;
		int dy = p2.second - p1.second;
		int xi = 1;
		if (dx < 0)
		{
			xi = -1;
			dx = -dx;
		}
		int D = (2 * dx) - dy;
		int x = p1.first;

		for (int y = p1.second; y < p2.second; y++)
		{
			if (D > 0)
			{
				x = x + xi;
				D = D + (2 * (dx - dy));
				steps += 2; // Can't take diagonal steps
			}
			else
			{
				D = D + 2 * dx;
				steps++;
			}
			input[y][x] = 'O';
		}
	}

	int Run(Input input)
	{
		std::cout << '\n';
		//PrintInput(input);

		// Detect empty rows
		std::vector<int> emptyRows;
		for (int i = 0; i < input.size(); i++)
		{
			const std::string& line = input[i];
			if (std::all_of(line.begin(), line.end(), [](int c) {return c == '.';})) emptyRows.push_back(i);
		}

		// Detect empty columns
		std::vector<int> emptyColumns;
		for (int x = 0; x < input[0].size(); x++)
		{
			bool isEmpty = true;
			for (int y = 0; y < input.size(); y++)
			{
				if (input[y][x] != '.') isEmpty = false;
			}
			if(isEmpty) emptyColumns.push_back(x);
		}

		// Create new input
		Input newInput;
		for (int y = 0; y < input[0].size(); y++)
		{
			std::string currRow = "";
			for (int x = 0; x < input.size(); x++)
			{
				currRow += input[y][x];
				// Push back new columns
				if (std::find(emptyColumns.begin(), emptyColumns.end(), x) != emptyColumns.end())
				{
					currRow += '.';
				}
			}
			newInput.push_back(currRow);

			// Push back entire rows
			if (std::find(emptyRows.begin(), emptyRows.end(), y) != emptyRows.end())
			{
				newInput.push_back(std::string(input.size() + emptyColumns.size(), '.'));
			}
		}

		//PrintInput(newInput);

		// Find galaxies and number them
		std::unordered_map<int, std::pair<int, int>> galaxyMap; // Maps a galaxy ID to it's 2D coordinate
		int galaxyCounter = 1;
		for (int y = 0; y < newInput.size(); y++)
		{
			auto& line = newInput[y];
			for (int x = 0; x < line.size(); x++)
			{
				auto& c = line[x];
				if (c == '#')
				{
					//c = '0' + galaxyCounter;
					galaxyMap.insert({ galaxyCounter, { x, y } });
					galaxyCounter++;
				}
			}
		}

		//PrintInput(newInput);

		// Generate combinations
		std::vector<std::pair<int, int>> combinations; // Stores combinations of galaxy IDs
		combinations.reserve(50000);

		int n = galaxyCounter - 1;
		int r = 2;
		int counter = 0;
		std::vector<bool> selectionArray(n);
		std::fill(selectionArray.end() - r, selectionArray.end(), true);
		do {
			std::pair<int, int> combination(-1, -1);
			for (int i = 0; i < n; ++i) {
				if (selectionArray[i]) {
					if (combination.first == -1)
					{
						combination.first = i + 1;
					}
					else
					{
						combination.second = i + 1;
					}
				}
			}
			combinations.push_back(combination);
		} while (std::next_permutation(selectionArray.begin(), selectionArray.end()));

		//for (auto combination : combinations)
		//{
		//	std::cout << combination.first << " " << combination.second << std::endl;
		//}
		//std::cout << "There are " << combinations.size() << " pairs!";

		// For all combinations, find euclidean distance and perform bresenham's line algorithm
		// This assumes that the line is drawn from left to right, and top to bottom, e.g. 
		// x1 < x2 and y1 < y2
		int distanceSum = 0;
		for (const auto& combination : combinations)
		{
			std::pair<int, int> p1, p2;
			p1 = galaxyMap.at(combination.first);
			p2 = galaxyMap.at(combination.second);
			int m = 2 * (p2.second - p1.second);
			int slopeError = m - (p2.first - p1.first);
			int currY = p1.second;
			int stepsTaken = 0;

			Input inputCopy = newInput;

			if (abs(p2.second - p1.second) < abs(p2.first - p1.first))
			{
				if (p1.first > p2.first)
				{
					BresenhamLow(p2, p1, stepsTaken, inputCopy);
				}
				else
				{
					BresenhamLow(p1, p2, stepsTaken, inputCopy);
				}
			}
			else
			{
				if (p1.second > p2.second)
				{
					BresenhamHigh(p2, p1, stepsTaken, inputCopy);
				}
				else
				{
					BresenhamHigh(p1, p2, stepsTaken, inputCopy);
				}
			}

			//PrintInput(inputCopy);

			//std::cout << "Distance between (" << p1.first << ", " << p1.second << ") and (" << p2.first << ", " << p2.second << ") is " << stepsTaken << std::endl;
			distanceSum += stepsTaken;
		}

		std::cout << "Actual result: " << distanceSum << std::endl;

		return distanceSum;
	}
};

constexpr int EMPTY_SPACE = 100;

struct Day11_2 : public Challenge
{
	void PrintInput(Input& input)
	{
		std::cout << "\n\n";
		for (const auto& line : input)
		{
			std::cout << line.c_str() << std::endl;
		}
		std::cout << "\n\n";
	}

	void BresenhamLow(std::pair<int, int> p1, std::pair<int, int> p2, int& steps, Input& input)
	{
		int dx = p2.first - p1.first;
		int dy = p2.second - p1.second;
		int yi = 1;
		if (dy < 0)
		{
			yi = -1;
			dy = -dy;
		}

		int D = (2 * dy) - dx;
		int y = p1.second;
		for (int x = p1.first; x < p2.first; x++)
		{
			char tileAfterMovement = input[y][x];
			if (D > 0) // vertical movement
			{
				y = y + yi;
				tileAfterMovement = input[y][x];
				D = D + (2 * (dy - dx));
				if (tileAfterMovement == 'X' || tileAfterMovement == 'Z' || tileAfterMovement == 'Y')
				{
					steps += EMPTY_SPACE + 1; // Can't take diagonal steps
				}
				else
				{
					steps += 2; // Can't take diagonal steps
				}
			}
			else // horizontal movement
			{
				D = D + 2 * dy;
				if(x + 1 < p2.first) tileAfterMovement = input[y][x + 1];
				if (tileAfterMovement == 'Y' || tileAfterMovement == 'Z')
				{
					steps += EMPTY_SPACE;
				}
				else
				{
					steps++;
				}
			}
			input[y][x] = 'O';
		}
	}

	void BresenhamHigh(std::pair<int, int> p1, std::pair<int, int> p2, int& steps, Input& input)
	{
		int dx = p2.first - p1.first;
		int dy = p2.second - p1.second;
		int xi = 1;
		if (dx < 0)
		{
			xi = -1;
			dx = -dx;
		}
		int D = (2 * dx) - dy;
		int x = p1.first;

		for (int y = p1.second; y < p2.second; y++)
		{
			char tileAfterMovement = input[y][x];
			if (D > 0) // horizontal movement
			{
				x = x + xi;
				tileAfterMovement = input[y][x];
				D = D + (2 * (dx - dy));
				if (tileAfterMovement == 'Y' || tileAfterMovement == 'Z' || tileAfterMovement == 'X')
				{
					steps += EMPTY_SPACE + 1;  // Can't take diagonal steps

				}
				else
				{
					steps += 2; // Can't take diagonal steps
				}
			}
			else // vertical movement
			{
				D = D + 2 * dx;
				if (y + 1 < p2.second) tileAfterMovement = input[y + 1][x];
				if (tileAfterMovement == 'X' || tileAfterMovement == 'Z')
				{
					steps += EMPTY_SPACE;
				}
				else
				{
					steps++;
				}
			}
			input[y][x] = 'O';
		}
	}

	int Run(Input input)
	{
		std::cout << '\n';
		PrintInput(input);

		// Detect empty rows
		std::vector<int> emptyRows;
		for (int i = 0; i < input.size(); i++)
		{
			const std::string& line = input[i];
			if (std::all_of(line.begin(), line.end(), [](int c) {return c == '.'; })) emptyRows.push_back(i);
		}

		// Detect empty columns
		std::vector<int> emptyColumns;
		for (int x = 0; x < input[0].size(); x++)
		{
			bool isEmpty = true;
			for (int y = 0; y < input.size(); y++)
			{
				if (input[y][x] != '.') isEmpty = false;
			}
			if (isEmpty) emptyColumns.push_back(x);
		}

		// Create new input by replacing empty rows + columns with letters
		for (int y = 0; y < input[0].size(); y++)
		{
			for (int x = 0; x < input.size(); x++)
			{
				// Replace column with 'Y'
				if (std::find(emptyColumns.begin(), emptyColumns.end(), x) != emptyColumns.end())
				{
					input[y][x] = 'Y';
				}
			}

			// Replace row with 'X', and any intersection with a column replace with a 'Z' instead
			if (std::find(emptyRows.begin(), emptyRows.end(), y) != emptyRows.end())
			{
				auto& affectedRow = input[y];
				for (auto& c : affectedRow)
				{
					if (c == '.')
					{
						c = 'X';
					}
					else
					{
						c = 'Z';
					}

				}
			}
		}

		PrintInput(input);

		// Find galaxies and number them
		std::unordered_map<int, std::pair<int, int>> galaxyMap; // Maps a galaxy ID to it's 2D coordinate
		int galaxyCounter = 1;
		for (int y = 0; y < input.size(); y++)
		{
			auto& line = input[y];
			for (int x = 0; x < line.size(); x++)
			{
				auto& c = line[x];
				if (c == '#')
				{
					//c = '0' + galaxyCounter;
					galaxyMap.insert({ galaxyCounter, { x, y } });
					galaxyCounter++;
				}
			}
		}

		//PrintInput(newInput);

		// Generate combinations
		std::vector<std::pair<int, int>> combinations; // Stores combinations of galaxy IDs
		combinations.reserve(50000);

		int n = galaxyCounter - 1;
		int r = 2;
		int counter = 0;
		std::vector<bool> selectionArray(n);
		std::fill(selectionArray.end() - r, selectionArray.end(), true);
		do {
			std::pair<int, int> combination(-1, -1);
			for (int i = 0; i < n; ++i) {
				if (selectionArray[i]) {
					if (combination.first == -1)
					{
						combination.first = i + 1;
					}
					else
					{
						combination.second = i + 1;
					}
				}
			}
			combinations.push_back(combination);
		} while (std::next_permutation(selectionArray.begin(), selectionArray.end()));

		//for (auto combination : combinations)
		//{
		//	std::cout << combination.first << " " << combination.second << std::endl;
		//}
		//std::cout << "There are " << combinations.size() << " pairs!";

		// For all combinations, find euclidean distance and perform bresenham's line algorithm
		// This assumes that the line is drawn from left to right, and top to bottom, e.g. 
		// x1 < x2 and y1 < y2
		int distanceSum = 0;
		for (const auto& combination : combinations)
		{
			std::pair<int, int> p1, p2;
			p1 = galaxyMap.at(combination.first);
			p2 = galaxyMap.at(combination.second);
			int m = 2 * (p2.second - p1.second);
			int slopeError = m - (p2.first - p1.first);
			int currY = p1.second;
			int stepsTaken = 0;

			Input inputCopy = input;

			if (abs(p2.second - p1.second) < abs(p2.first - p1.first))
			{
				if (p1.first > p2.first)
				{
					BresenhamLow(p2, p1, stepsTaken, inputCopy);
				}
				else
				{
					BresenhamLow(p1, p2, stepsTaken, inputCopy);
				}
			}
			else
			{
				if (p1.second > p2.second)
				{
					BresenhamHigh(p2, p1, stepsTaken, inputCopy);
				}
				else
				{
					BresenhamHigh(p1, p2, stepsTaken, inputCopy);
				}
			}

			PrintInput(inputCopy);

			std::cout << "Distance between (" << p1.first << ", " << p1.second << ") and (" << p2.first << ", " << p2.second << ") is " << stepsTaken << std::endl;
			distanceSum += stepsTaken;
		}

		std::cout << "Actual result: " << distanceSum << std::endl;

		return distanceSum;
	}
};