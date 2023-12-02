// -- - Day 1: Trebuchet ? !-- -
// Something is wrong with global snow production, and you've been selected to take a look. The Elves have even given you a map; on it, they've used stars to mark the top fifty locations that are likely to be having problems.
// 
// You've been doing this long enough to know that to restore snow operations, you need to check all fifty stars by December 25th.
// 
// Collect stars by solving puzzles.Two puzzles will be made available on each day in the Advent calendar; the second puzzle is unlocked when you complete the first.Each puzzle grants one star.Good luck!
// 
// You try to ask why they can't just use a weather machine ("not powerful enough") and where they're even sending you("the sky") and why your map looks mostly blank("you sure ask a lot of questions") and hang on did you just say the sky("of course, where do you think snow comes from") when you realize that the Elves are already loading you into a trebuchet("please hold still, we need to strap you in").
// 
// As they're making the final adjustments, they discover that their calibration document (your puzzle input) has been amended by a very young Elf who was apparently just excited to show off her art skills. Consequently, the Elves are having trouble reading the values on the document.
// 
// The newly - improved calibration document consists of lines of text; each line originally contained a specific calibration value that the Elves now need to recover.On each line, the calibration value can be found by combining the first digitand the last digit(in that order) to form a single two - digit number.
// 
// For example :
// 
// 1abc2
// pqr3stu8vwx
// a1b2c3d4e5f
// treb7uchet
// In this example, the calibration values of these four lines are 12, 38, 15, and 77. Adding these together produces 142.
// 
// Consider your entire calibration document.What is the sum of all of the calibration values ?

#include <unordered_map>

#include "../challenge.h"

struct Day1_1 : public Challenge
{
	int Run(std::vector<std::string> input)
	{
		int sum = 0;

		for (auto line : input)
		{
			int first = -1;
			int last = -1;
			for (auto character : line)
			{
				if (std::isdigit(character))
				{
					// NOTE - Maybe we could do something clever with this function returning 0?
					int digit = atoi(&character);

					if (first == -1)
					{
						first = digit;
						last = digit;
					}

					last = digit;
				}
			}

			sum += first * 10 + last;
		}

		return sum;
	}
};

static const std::unordered_map<std::string, int> STRING_TO_NUMBER =
{
	{ "one", 1 },
	{ "two", 2 },
	{ "three", 3 },
	{ "four", 4 },
	{ "five", 5 },
	{ "six", 6 },
	{ "seven", 7 },
	{ "eight", 8 },
	{ "nine", 9 },
};

struct Day1_2 : public Challenge
{
	int Run(std::vector<std::string> input)
	{
		int sum = 0;

		for (auto line : input)
		{
			int first = -1;
			int last = -1;
			for (int i = 0; i < line.size(); i++)
			{
				char character = line[i];
				if (std::isdigit(character))
				{
					int digit = atoi(&character);

					if (first == -1)
					{
						first = digit;
					}

					last = digit;
				}
				else
				{
					for (auto iter : STRING_TO_NUMBER)
					{
						int pos = line.find(iter.first, i);
						if (pos == i && pos != std::string::npos)
						{
							if (first == -1)
							{
								first = iter.second;
							}

							last = iter.second;
							break;
						}
					}
				}
			}
			sum += first * 10 + last;
		}

		return sum;
	}
};