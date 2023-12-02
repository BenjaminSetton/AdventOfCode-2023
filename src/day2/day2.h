// -- - Day 2: Cube Conundrum-- -
// You're launched high into the atmosphere! The apex of your trajectory just barely reaches the surface of a large island floating in the sky. You gently land in a fluffy pile of leaves. It's quite cold, but you don't see much snow. An Elf runs over to greet you.
// 
// The Elf explains that you've arrived at Snow Island and apologizes for the lack of snow. He'll be happy to explain the situation, but it's a bit of a walk, so you have some time. They don't get many visitors up here; would you like to play a game in the meantime ?
// 
// As you walk, the Elf shows you a small bagand some cubes which are either red, green, or blue.Each time you play this game, he will hide a secret number of cubes of each color in the bag, and your goal is to figure out information about the number of cubes.
// 
// To get information, once a bag has been loaded with cubes, the Elf will reach into the bag, grab a handful of random cubes, show them to you, and then put them back in the bag.He'll do this a few times per game.
// 
// You play several games and record the information from each game(your puzzle input).Each game is listed with its ID number(like the 11 in Game 11: ...) followed by a semicolon - separated list of subsets of cubes that were revealed from the bag(like 3 red, 5 green, 4 blue).
// 
// For example, the record of a few games might look like this:
// 
// Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
// Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
// Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
// Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
// Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
// In game 1, three sets of cubes are revealed from the bag(and then put back again).The first set is 3 blue cubes and 4 red cubes; the second set is 1 red cube, 2 green cubes, and 6 blue cubes; the third set is only 2 green cubes.
// 
// The Elf would first like to know which games would have been possible if the bag contained only 12 red cubes, 13 green cubes, and 14 blue cubes ?
// 
// In the example above, games 1, 2, and 5 would have been possible if the bag had been loaded with that configuration.However, game 3 would have been impossible because at one point the Elf showed you 20 red cubes at once; similarly, game 4 would also have been impossible because the Elf showed you 15 blue cubes at once.If you add up the IDs of the games that would have been possible, you get 8.
// 
// Determine which games would have been possible if the bag had been loaded with only 12 red cubes, 13 green cubes, and 14 blue cubes.What is the sum of the IDs of those games ?
// 
// To begin, get your puzzle input

#include <assert.h>
#include <unordered_map>

#include "../challenge.h"

class Game
{
public:

	bool IsPossible(std::string input)
	{
		int colonPos = input.find(":");

		std::string id_str = input.substr(5, colonPos - 5);
		for(const auto c : id_str) assert(std::isdigit(c));

		id = atoi(id_str.c_str());
		
		input.erase(0, colonPos + 1);

		// Since the cubes are put back after each draw, then all we care about is if the picked number of cubes of a certain color is greater
		// than the ones stated in the question (this would make the game impossible)
		static const std::unordered_map<std::string, int> DRAW_LIMITS =
		{
			{ "red", 12 },
			{ "green", 13 },
			{ "blue", 14 }
		};

		std::vector<std::string> draws;
		char delimiter = ';';
		int delimiterPos = 0;
		while ((delimiterPos = input.find(delimiter)) != std::string::npos)
		{
			draws.push_back(input.substr(1, delimiterPos));
			input.erase(0, delimiterPos + 1);
		}

		// Get the last draw
		if (!input.empty())
		{
			draws.push_back(input.substr(1, input.size()));
			input.clear();
		}

		for (const auto draw : draws)
		{
			int redPicked = 0;
			int greenPicked = 0;
			int bluePicked = 0;

			int number = 0;
			std::string number_str = "";
			for (int i = 0; i < draw.size(); i++)
			{
				char c = draw[i];
				if (!std::isdigit(c))
				{
					if (!number_str.empty())
					{
						number = std::atoi(number_str.c_str());
						number_str.clear();
					}
				}

				if (c == 'r')
				{
					redPicked = number;
					number = 0;
					i += 3;
				}
				else if (c == 'g')
				{
					greenPicked = number;
					number = 0;
					i += 5;
				}
				else if (c == 'b')
				{
					bluePicked = number;
					number = 0;
					i += 4;
				}
				else if (std::isdigit(c))
				{
					number_str += c;
				}
			}

			if (redPicked > DRAW_LIMITS.at("red") ||
				greenPicked > DRAW_LIMITS.at("green") ||
				bluePicked > DRAW_LIMITS.at("blue"))
			{
				// Draw is illegal
				return false;
			}
		}

		return true;
	}

	void MinCubesRequiredToPlay(std::string input, int* redCubes, int* greenCubes, int* blueCubes)
	{
		int colonPos = input.find(":");

		std::string id_str = input.substr(5, colonPos - 5);
		for (const auto c : id_str) assert(std::isdigit(c));

		id = atoi(id_str.c_str());

		input.erase(0, colonPos + 1);

		std::vector<std::string> draws;
		char delimiter = ';';
		int delimiterPos = 0;
		while ((delimiterPos = input.find(delimiter)) != std::string::npos)
		{
			draws.push_back(input.substr(1, delimiterPos));
			input.erase(0, delimiterPos + 1);
		}

		// Get the last draw
		if (!input.empty())
		{
			draws.push_back(input.substr(1, input.size()));
			input.clear();
		}

		// Keep track of the highest number of each color
		int redMax, greenMax, blueMax;
		for (const auto draw : draws)
		{
			int number = 0;
			std::string number_str = "";
			for (int i = 0; i < draw.size(); i++)
			{
				char c = draw[i];
				if (!std::isdigit(c))
				{
					if (!number_str.empty())
					{
						number = std::atoi(number_str.c_str());
						number_str.clear();
					}
				}

				if (c == 'r')
				{
					redMax = std::max(redMax, number);
					number = 0;
					i += 3;
				}
				else if (c == 'g')
				{
					greenMax = std::max(greenMax, number);
					number = 0;
					i += 5;
				}
				else if (c == 'b')
				{
					blueMax = std::max(blueMax, number);
					number = 0;
					i += 4;
				}
				else if (std::isdigit(c))
				{
					number_str += c;
				}
			}
		}

		// Return the output
		*redCubes = redMax;
		*greenCubes = greenMax;
		*blueCubes = blueMax;
	}

	int GetID() const { return id; }

private:

	int id;
};

struct Day2_1 : public Challenge
{
	int Run(Input input)
	{
		// Question: which games would have been possible if the bag contained only 12 red cubes, 13 green cubes, and 14 blue cubes
		int IDSum = 0;
		for (auto line : input)
		{
			Game game;
			bool possible = game.IsPossible(line);
			if (possible)
			{
				std::cout << game.GetID() << " - [PASS] - " << line << std::endl;
				IDSum += game.GetID();
			}
			else
			{
				std::cout << game.GetID() << " - [FAIL] - " << line << std::endl;
			}
		}

		return IDSum;
	}
};

// --- Part Two ---
// The Elf says they've stopped producing snow because they aren't getting any water! He isn't sure why the water stopped; however, he can show you how to get to the water source to check it out for yourself. It's just up ahead!
// 
// As you continue your walk, the Elf poses a second question: in each game you played, what is the fewest number of cubes of each color that could have been in the bag to make the game possible?
// 
// Again consider the example games from earlier:
// 
// Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
// Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
// Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
// Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
// Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
// In game 1, the game could have been played with as few as 4 red, 2 green, and 6 blue cubes. If any color had even one fewer cube, the game would have been impossible.
// Game 2 could have been played with a minimum of 1 red, 3 green, and 4 blue cubes.
// Game 3 must have been played with at least 20 red, 13 green, and 6 blue cubes.
// Game 4 required at least 14 red, 3 green, and 15 blue cubes.
// Game 5 needed no fewer than 6 red, 3 green, and 2 blue cubes in the bag.
// The power of a set of cubes is equal to the numbers of red, green, and blue cubes multiplied together. The power of the minimum set of cubes in game 1 is 48. In games 2-5 it was 12, 1560, 630, and 36, respectively. Adding up these five powers produces the sum 2286.
// 
// For each game, find the minimum set of cubes that must have been present. What is the sum of the power of these sets?

struct Day2_2 : public Challenge
{
	int Run(Input input)
	{
		// Question: what is the fewest number of cubes of each color that could have been in the bag to make the game possible?
		int red, green, blue;
		int powerSum = 0;

		Game game;
		for (const auto line : input)
		{
			int power = 0;
			game.MinCubesRequiredToPlay(line, &red, &green, &blue);

			std::cout << red << "R " << green << "G " << blue << "B - " << line << std::endl;

			power = red * green * blue;
			powerSum += power;
		}

		return powerSum;
	}
};