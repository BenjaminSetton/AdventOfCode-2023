// --- Day 9: Mirage Maintenance ---
// You ride the camel through the sandstorm and stop where the ghost's maps told you to stop. The sandstorm subsequently subsides, somehow seeing you standing at an oasis!
// 
// The camel goes to get some water and you stretch your neck. As you look up, you discover what must be yet another giant floating island, this one made of metal! That must be where the parts to fix the sand machines come from.
// 
// There's even a hang glider partially buried in the sand here; once the sun rises and heats up the sand, you might be able to use the glider and the hot air to get all the way up to the metal island!
// 
// While you wait for the sun to rise, you admire the oasis hidden here in the middle of Desert Island. It must have a delicate ecosystem; you might as well take some ecological readings while you wait. Maybe you can report any environmental instabilities you find to someone so the oasis can be around for the next sandstorm-worn traveler.
// 
// You pull out your handy Oasis And Sand Instability Sensor and analyze your surroundings. The OASIS produces a report of many values and how they are changing over time (your puzzle input). Each line in the report contains the history of a single value. For example:
// 
// 0 3 6 9 12 15
// 1 3 6 10 15 21
// 10 13 16 21 30 45
// To best protect the oasis, your environmental report should include a prediction of the next value in each history. To do this, start by making a new sequence from the difference at each step of your history. If that sequence is not all zeroes, repeat this process, using the sequence you just generated as the input sequence. Once all of the values in your latest sequence are zeroes, you can extrapolate what the next value of the original history should be.
// 
// In the above dataset, the first history is 0 3 6 9 12 15. Because the values increase by 3 each step, the first sequence of differences that you generate will be 3 3 3 3 3. Note that this sequence has one fewer value than the input sequence because at each step it considers two numbers from the input. Since these values aren't all zero, repeat the process: the values differ by 0 at each step, so the next sequence is 0 0 0 0. This means you have enough information to extrapolate the history! Visually, these sequences can be arranged like this:
// 
// 0   3   6   9  12  15
//   3   3   3   3   3
//     0   0   0   0
// To extrapolate, start by adding a new zero to the end of your list of zeroes; because the zeroes represent differences between the two values above them, this also means there is now a placeholder in every sequence above it:
// 
// 0   3   6   9  12  15   B
//   3   3   3   3   3   A
//     0   0   0   0   0
// You can then start filling in placeholders from the bottom up. A needs to be the result of increasing 3 (the value to its left) by 0 (the value below it); this means A must be 3:
// 
// 0   3   6   9  12  15   B
//   3   3   3   3   3   3
//     0   0   0   0   0
// Finally, you can fill in B, which needs to be the result of increasing 15 (the value to its left) by 3 (the value below it), or 18:
// 
// 0   3   6   9  12  15  18
//   3   3   3   3   3   3
//     0   0   0   0   0
// So, the next value of the first history is 18.
// 
// Finding all-zero differences for the second history requires an additional sequence:
// 
// 1   3   6  10  15  21
//   2   3   4   5   6
//     1   1   1   1
//       0   0   0
// Then, following the same process as before, work out the next value in each sequence from the bottom up:
// 
// 1   3   6  10  15  21  28
//   2   3   4   5   6   7
//     1   1   1   1   1
//       0   0   0   0
// So, the next value of the second history is 28.
// 
// The third history requires even more sequences, but its next value can be found the same way:
// 
// 10  13  16  21  30  45  68
//    3   3   5   9  15  23
//      0   2   4   6   8
//        2   2   2   2
//          0   0   0
// So, the next value of the third history is 68.
// 
// If you find the next value for each history in this example and add them together, you get 114.
// 
// Analyze your OASIS report and extrapolate the next value for each history. What is the sum of these extrapolated values?

#include "../challenge.h"

#include <assert.h>
#include <numeric>
#include <vector>

typedef int64_t NumType;

struct Day9_1 : public Challenge
{
	// Reads the line and appends the parsed numbers into out_arr, unless it's nullptr.
	// Returns the number of integers read
	uint64_t ReadData(const std::string& line, uint64_t maxLen, NumType* out_arr)
	{
		std::string numberStr = "";
		size_t index = 0;
		uint64_t numbersRead = 0;
		for (const auto& c : line)
		{
			if (c == ' ')
			{
				if (!numberStr.empty())
				{
					numbersRead++;
					if(out_arr != nullptr && index < maxLen)
					{
						out_arr[index++] = std::stoull(numberStr);
						numberStr.clear();
					}
				}

				continue;
			}

			numberStr += c;
		}

		// Get last number
		if (!numberStr.empty())
		{
			numbersRead++;
			if (out_arr != nullptr && index < maxLen)
			{
				out_arr[index++] = std::stoull(numberStr);
				numberStr.clear();
			}
		}

		return numbersRead;
	}

	bool IsZero(const NumType* arr, uint64_t len)
	{
		for (int i = 0; i < len; i++)
		{
			if (arr[i] != 0ll) return false;
		}
		return true;
	}

	void SwapBuffers(NumType** a, NumType** b)
	{
		NumType* temp = *a;
		*a = *b;
		*b = temp;
	}

	void ZeroBuffer(NumType* a, uint64_t len)
	{
		memset(a, 0, len * sizeof(NumType));
	}

	int Run(Input input)
	{
		int64_t sum = 0;
		for (const auto& line : input)
		{
			auto numbersRead = ReadData(line, 0, nullptr);

			NumType* readBuff = new NumType[numbersRead + 1]; // Accomodate for the next number
			ReadData(line, numbersRead, readBuff);

			NumType* writeBuff = new NumType[numbersRead + 1]; // Accomodate for the next number
			ReadData(line, numbersRead, writeBuff);

			NumType highestStartingNumber = readBuff[numbersRead - 1];

			std::vector<NumType> startNumbers;
			startNumbers.reserve(50);
			startNumbers.push_back(readBuff[0]);

			int depth = 0;
			// Go down to the zero-array
			while (!IsZero(writeBuff, numbersRead))
			{
				// Switch write buffer
				SwapBuffers(&readBuff, &writeBuff);

				// Fill out the next depth
				ZeroBuffer(writeBuff, numbersRead + 1);
				for (int i = (numbersRead - 1) - depth; i >= 0; i--)
				{
					if (i - 1 >= 0)
					{
						writeBuff[i - 1] = readBuff[i] - readBuff[i - 1];
					}
				}

				// Cache the newest starting number
				startNumbers.push_back(writeBuff[0]);

				depth++;
			}

			assert(startNumbers.size() == depth + 1);

			// Add the new number to the constant depth (every number is the same)
			readBuff[numbersRead - depth + 1] = readBuff[0];

			// Go back up to find the new number
			int currentDepth = depth - 2;
			for (int i = currentDepth; i >= 0; i--)
			{
				writeBuff[0] = startNumbers[i]; // Skip lowest depth (zero buffer) and the buffer we're reading from
				for (int j = 1; j < numbersRead - currentDepth + 1; j++)
				{
					writeBuff[j] = readBuff[j - 1] + writeBuff[j - 1];
				}

				SwapBuffers(&readBuff, &writeBuff);

				currentDepth--;
			}

			assert(readBuff[numbersRead - 1] == highestStartingNumber);

			NumType extrapolatedNumber = readBuff[numbersRead];
			sum += extrapolatedNumber;

			// Cleanup
			delete[] readBuff;
			delete[] writeBuff;
		}

		std::cout << "Actual result: " << sum << std::endl;

		return sum;
	}
};

// --- Part Two ---
// Of course, it would be nice to have even more history included in your report. Surely it's safe to just extrapolate backwards as well, right?
// 
// For each history, repeat the process of finding differences until the sequence of differences is entirely zero. Then, rather than adding a zero to the end and filling in the next values of each previous sequence, you should instead add a zero to the beginning of your sequence of zeroes, then fill in new first values for each previous sequence.
// 
// In particular, here is what the third example history looks like when extrapolating back in time:
// 
// 5  10  13  16  21  30  45
//   5   3   3   5   9  15
//    -2   0   2   4   6
//       2   2   2   2
//         0   0   0
// Adding the new values on the left side of each sequence from bottom to top eventually reveals the new left-most history value: 5.
// 
// Doing this for the remaining example data above results in previous values of -3 for the first history and 0 for the second history. Adding all three new values together produces 2.
// 
// Analyze your OASIS report again, this time extrapolating the previous value for each history. What is the sum of these extrapolated values?

struct Day9_2 : public Challenge
{
	// Reads the line and appends the parsed numbers into out_arr, unless it's nullptr.
	// Returns the number of integers read
	uint64_t ReadData(const std::string& line, uint64_t maxLen, NumType* out_arr)
	{
		std::string numberStr = "";
		size_t index = 0;
		uint64_t numbersRead = 0;
		for (const auto& c : line)
		{
			if (c == ' ')
			{
				if (!numberStr.empty())
				{
					numbersRead++;
					if (out_arr != nullptr && index < maxLen)
					{
						out_arr[index++] = std::stoull(numberStr);
						numberStr.clear();
					}
				}

				continue;
			}

			numberStr += c;
		}

		// Get last number
		if (!numberStr.empty())
		{
			numbersRead++;
			if (out_arr != nullptr && index < maxLen)
			{
				out_arr[index++] = std::stoull(numberStr);
				numberStr.clear();
			}
		}

		return numbersRead;
	}

	bool IsZero(const NumType* arr, uint64_t len)
	{
		for (int i = 0; i < len; i++)
		{
			if (arr[i] != 0ll) return false;
		}
		return true;
	}

	void SwapBuffers(NumType** a, NumType** b)
	{
		NumType* temp = *a;
		*a = *b;
		*b = temp;
	}

	void ZeroBuffer(NumType* a, uint64_t len)
	{
		memset(a, 0, len * sizeof(NumType));
	}

	int Run(Input input)
	{
		int64_t sum = 0;
		for (const auto& line : input)
		{
			auto numbersRead = ReadData(line, 0, nullptr);

			NumType* readBuff = new NumType[numbersRead + 1]; // Accomodate for the next number
			ReadData(line, numbersRead, readBuff);

			NumType* writeBuff = new NumType[numbersRead + 1]; // Accomodate for the next number
			ReadData(line, numbersRead, writeBuff);

			NumType highestStartingNumber = readBuff[numbersRead - 1];

			std::vector<NumType> endNumbers;
			endNumbers.reserve(50);
			endNumbers.push_back(highestStartingNumber);

			int depth = 0;
			// Go down to the zero-array
			while (!IsZero(writeBuff, numbersRead))
			{
				// Switch write buffer
				SwapBuffers(&readBuff, &writeBuff);

				// Fill out the next depth
				ZeroBuffer(writeBuff, numbersRead + 1);
				for (int i = (numbersRead - 1) - depth; i >= 0; i--)
				{
					if (i - 1 >= 0)
					{
						writeBuff[i - 1] = readBuff[i] - readBuff[i - 1];
					}
				}

				// Cache the newest starting number
				endNumbers.push_back(writeBuff[(numbersRead - 2) - depth]);

				depth++;
			}

			assert(endNumbers.size() == depth + 1);

			// Add the new number to the constant depth (every number is the same)
			readBuff[numbersRead - depth + 1] = readBuff[0];

			int currentDepth = depth - 2;

			// Go back up to find the new number
			for (int i = currentDepth; i >= 0; i--)
			{
				writeBuff[0] = endNumbers[i]; // Skip lowest depth (zero buffer) and the buffer we're reading from
				for (int j = 1; j < numbersRead - currentDepth + 1; j++)
				{
					writeBuff[j] = writeBuff[j - 1] - readBuff[j - 1];
				}

				SwapBuffers(&readBuff, &writeBuff);

				currentDepth--;
			}

			NumType extrapolatedNumber = readBuff[numbersRead];
			sum += extrapolatedNumber;

			// Cleanup
			delete[] readBuff;
			delete[] writeBuff;
		}

		std::cout << "Actual result: " << sum << std::endl;

		return sum;
	}
};