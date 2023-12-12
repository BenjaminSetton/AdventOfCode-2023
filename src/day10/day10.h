// --- Day 10: Pipe Maze ---
// You use the hang glider to ride the hot air from Desert Island all the way up to the floating metal island. This island is surprisingly cold and there definitely aren't any thermals to glide on, so you leave your hang glider behind.
// 
// You wander around for a while, but you don't find any people or animals. However, you do occasionally find signposts labeled "Hot Springs" pointing in a seemingly consistent direction; maybe you can find someone at the hot springs and ask them where the desert-machine parts are made.
// 
// The landscape here is alien; even the flowers and trees are made of metal. As you stop to admire some metal grass, you notice something metallic scurry away in your peripheral vision and jump into a big pipe! It didn't look like any animal you've ever seen; if you want a better look, you'll need to get ahead of it.
// 
// Scanning the area, you discover that the entire field you're standing on is densely packed with pipes; it was hard to tell at first because they're the same metallic silver color as the "ground". You make a quick sketch of all of the surface pipes you can see (your puzzle input).
// 
// The pipes are arranged in a two-dimensional grid of tiles:
// 
// | is a vertical pipe connecting north and south.
// - is a horizontal pipe connecting east and west.
// L is a 90-degree bend connecting north and east.
// J is a 90-degree bend connecting north and west.
// 7 is a 90-degree bend connecting south and west.
// F is a 90-degree bend connecting south and east.
// . is ground; there is no pipe in this tile.
// S is the starting position of the animal; there is a pipe on this tile, but your sketch doesn't show what shape the pipe has.
// Based on the acoustics of the animal's scurrying, you're confident the pipe that contains the animal is one large, continuous loop.
// 
// For example, here is a square loop of pipe:
// 
// .....
// .F-7.
// .|.|.
// .L-J.
// .....
// If the animal had entered this loop in the northwest corner, the sketch would instead look like this:
// 
// .....
// .S-7.
// .|.|.
// .L-J.
// .....
// In the above diagram, the S tile is still a 90-degree F bend: you can tell because of how the adjacent pipes connect to it.
// 
// Unfortunately, there are also many pipes that aren't connected to the loop! This sketch shows the same loop as above:
// 
// -L|F7
// 7S-7|
// L|7||
// -L-J|
// L|-JF
// In the above diagram, you can still figure out which pipes form the main loop: they're the ones connected to S, pipes those pipes connect to, pipes those pipes connect to, and so on. Every pipe in the main loop connects to its two neighbors (including S, which will have exactly two pipes connecting to it, and which is assumed to connect back to those two pipes).
// 
// Here is a sketch that contains a slightly more complex main loop:
// 
// ..F7.
// .FJ|.
// SJ.L7
// |F--J
// LJ...
// Here's the same example sketch with the extra, non-main-loop pipe tiles also shown:
// 
// 7-F7-
// .FJ|7
// SJLL7
// |F--J
// LJ.LJ
// If you want to get out ahead of the animal, you should find the tile in the loop that is farthest from the starting position. Because the animal is in the pipe, it doesn't make sense to measure this by direct distance. Instead, you need to find the tile that would take the longest number of steps along the loop to reach from the starting point - regardless of which way around the loop the animal went.
// 
// In the first example with the square loop:
// 
// .....
// .S-7.
// .|.|.
// .L-J.
// .....
// You can count the distance each tile in the loop is from the starting point like this:
// 
// .....
// .012.
// .1.3.
// .234.
// .....
// In this example, the farthest point from the start is 4 steps away.
// 
// Here's the more complex loop again:
// 
// ..F7.
// .FJ|.
// SJ.L7
// |F--J
// LJ...
// Here are the distances for each tile on that loop:
// 
// ..45.
// .236.
// 01.78
// 14567
// 23...
// Find the single giant loop starting at S. How many steps along the loop does it take to get from the starting position to the point farthest from the starting position?


#include "../challenge.h"

#include <assert.h>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <vector>

struct Vec2
{
	Vec2(int _x, int _y) : x(_x), y(_y)
	{
	}

	~Vec2() {}

	Vec2(const Vec2& other) : x(other.x), y(other.y)
	{
	}

	// Operator overloads
	Vec2& operator=(const Vec2& other)
	{
		if (this == &other)
		{
			return *this;
		}

		x = other.x;
		y = other.y;

		return *this;
	}

	Vec2& operator+=(const Vec2& other)
	{
		if (this == &other)
		{
			return *this;
		}

		x += other.x;
		y += other.y;

		return *this;
	}

	Vec2& operator-=(const Vec2& other)
	{
		if (this == &other)
		{
			return *this;
		}

		x -= other.x;
		y -= other.y;

		return *this;
	}

	Vec2 operator+(const Vec2& other)
	{
		Vec2 res(x, y);
		res.x += other.x;
		res.y += other.y;

		return res;
	}


	int x, y;
};

constexpr bool operator==(const Vec2& lhs, const Vec2& rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

// A tile is just a 2D vector in a grid, where a rightward direction is positive X and a downwards direction is positive Y. For example:
// |
// |  -Y
// |  ^
// |  |
// |  |
// |  v
// |  +Y
// |       -X <--> +X
//  - - - - - - - - - - -
typedef Vec2 Tile;

// Maps a character to the two tiles it connects, assuming the current tile
// (key) is the starting tile (e.g. [0, 0])
const std::unordered_map<char, std::vector<Tile>> CharToTile =
{
	{ '|', { Vec2(0, 1), Vec2(0, -1) } },
	{ '-', { Vec2(1, 0), Vec2(-1, 0) } },
	{ 'L', { Vec2(0, -1), Vec2(1, 0) } },
	{ 'J', { Vec2(0, -1), Vec2(-1, 0) } },
	{ '7', { Vec2(-1, 0), Vec2(0, 1) } },
	{ 'F', { Vec2(1, 0), Vec2(0, 1) } },
	{ 'S', { Vec2(1, 0), Vec2(-1, 0), Vec2(0, 1), Vec2(0, -1) } } // Assume 'S' connect to everything
};

const std::vector<Tile> NeighborKernel =
{
	{ Tile(0, -1) }, // Down
	{ Tile(0, 1) }, // Up
	{ Tile(1, 0) }, // Right
	{ Tile(-1, 0) }, // Left
};


struct Day10_1 : public Challenge
{
	bool IsClosedLoop(const std::vector<Tile>& maze)
	{
		if (maze.size() == 0) return false;

		Tile start = maze[0];
		Tile current = maze[0]; // start at 'S'
		for (const auto& tile : maze)
		{
			current += tile;
		}

		return start == current;
	}

	std::optional<char> GetTileType(Vec2 neighborPosition, const Input* input)
	{
		if (input->size() == 0) return {};

		size_t gridWidth = (*input)[0].size();
		size_t gridHeight = input->size();
		if (neighborPosition.x >= gridWidth || neighborPosition.y >= gridHeight)
		{
			// Out of bounds
			return {};
		}

		char type = (*input)[neighborPosition.y][neighborPosition.x];
		// Ignore '.' in the input
		if (type == '.') return {};

		return type;
	}

	bool IsValidConnection(char currentType, Tile neighborOffset, char neighborType, bool forceConnectionToNeighbor = false)
	{
		// If this flag is set to true, we'll assume that the current tile connects to the neighbor. This is used
		// when calculating the starting tile, since we don't know what tile it is yet
		if (!forceConnectionToNeighbor)
		{
			const auto& validConnections = CharToTile.at(currentType);
			if (std::find(validConnections.begin(), validConnections.end(), neighborOffset) == validConnections.end())
			{
				// We don't connect to the neighbor
				return false;
			}
		}

		const auto& neighborConnections = CharToTile.at(neighborType);
		for (const auto& connection : neighborConnections)
		{
			// It's only a valid connection if the neighbor's type connects opposite to one of our connections
			// e.g. we connect East + South and the neighbor connects North + West. This is valid because we connect
			// South ([0, -1]) and the neighbor North ([0, 1]), meaning the sum of the two vectors is a zero vector
			if (neighborOffset + connection == Vec2(0, 0))
			{
				return true;
			}
		}

		return false;
	}

	int Run(Input input)
	{
		// Represent indices into the input 'grid'
		Vec2 startingPoint(-1, -1);

		for (int i = 0; i < input.size(); i++)
		{
			const auto& line = input[i];

			startingPoint.x = line.find('S');
			if (startingPoint.x == std::string::npos)
			{
				continue;
			}

			// We found the starting point, cache Y index
			startingPoint.y = i;
			break;
		}

		// The maze contains a list of offsets starting from the starting point (not included in the maze). The index of the starting
		// point in the input is cached in startingPoint above
		std::vector<Tile> maze;
		maze.reserve(100);
		std::vector<Tile> validNeighbors;
		validNeighbors.reserve(4);

		// Calculate potential neighbors for starting point
		for (const Tile& neighbor : NeighborKernel)
		{
			auto neighborType = GetTileType(startingPoint + neighbor, &input);
			if (neighborType.has_value())
			{
				if (IsValidConnection('.', neighbor, neighborType.value(), true))
				{
					validNeighbors.push_back(neighbor);
				}

			}
		}

		// Run through every potential candidate for the starting point
		for (const Tile& startNeighborCandidate : validNeighbors)
		{
			// If the last tile in our closed maze is one of the neighbor candidates, then ignore it
			if (!maze.empty())
			{
				if (maze[maze.size() - 1] == Vec2(-startNeighborCandidate.x, -startNeighborCandidate.y))
				{
					continue;
				}
			}

			std::vector<Tile> currentNeighbors;
			currentNeighbors.reserve(4);
			currentNeighbors.push_back(startNeighborCandidate);

			maze.push_back(startNeighborCandidate);

			Vec2 previousPoint = startingPoint;
			Vec2 currentPoint = startingPoint + startNeighborCandidate;

			// Calculate the next neighbor and fill out the maze of tiles
			while (!IsClosedLoop(maze))
			{
				std::optional<char> currentType = GetTileType(currentPoint, &input);
				assert(currentType.has_value());

				// Calculate potential neighbors for current point
				currentNeighbors.clear();
				for (const Tile& neighbor : NeighborKernel)
				{
					// Skip counting the previous tile as a valid neighbor, since we just came from that tile
					if (currentPoint + neighbor == previousPoint) continue;

					auto neighborType = GetTileType(currentPoint + neighbor, &input);
					if (neighborType.has_value())
					{
						if (IsValidConnection(currentType.value(), neighbor, neighborType.value()))
						{
							currentNeighbors.push_back(neighbor);
						}
					}
				}

				if (currentNeighbors.empty())
				{
					// We know this candidate won't work because we reached a dead-end
					maze.clear();
					break;
				}

				// Consider branches??
				assert(currentNeighbors.size() == 1);

				Vec2 firstNeighbor = currentNeighbors[0];
				maze.push_back(firstNeighbor);

				// Update tracking variables
				previousPoint = currentPoint;
				currentPoint += firstNeighbor;
			}
		}

		// We now have a closed loop, calculate furthest tile (mid-point)
		size_t stepsToFurthestTile = std::ceil(maze.size() / 2);

		std::cout << "Maze size: " << maze.size() << " | Steps to furthest tile: " << stepsToFurthestTile << std::endl;
		std::cout << "Actual result: " << stepsToFurthestTile << std::endl;

		return stepsToFurthestTile;
	}
};

// --- Part Two ---
// You quickly reach the farthest point of the loop, but the animal never emerges. Maybe its nest is within the area enclosed by the loop?
// 
// To determine whether it's even worth taking the time to search for such a nest, you should calculate how many tiles are contained within the loop. For example:
// 
// ...........
// .S-------7.
// .|F-----7|.
// .||.....||.
// .||.....||.
// .|L-7.F-J|.
// .|..|.|..|.
// .L--J.L--J.
// ...........
// The above loop encloses merely four tiles - the two pairs of . in the southwest and southeast (marked I below). The middle . tiles (marked O below) are not in the loop. Here is the same loop again with those regions marked:
// 
// ...........
// .S-------7.
// .|F-----7|.
// .||OOOOO||.
// .||OOOOO||.
// .|L-7OF-J|.
// .|II|O|II|.
// .L--JOL--J.
// .....O.....
// In fact, there doesn't even need to be a full tile path to the outside for tiles to count as outside the loop - squeezing between pipes is also allowed! Here, I is still within the loop and O is still outside the loop:
// 
// ..........
// .S------7.
// .|F----7|.
// .||OOOO||.
// .||OOOO||.
// .|L-7F-J|.
// .|II||II|.
// .L--JL--J.
// ..........
// In both of the above examples, 4 tiles are enclosed by the loop.
// 
// Here's a larger example:
// 
// .F----7F7F7F7F-7....
// .|F--7||||||||FJ....
// .||.FJ||||||||L7....
// FJL7L7LJLJ||LJ.L-7..
// L--J.L7...LJS7F-7L7.
// ....F-J..F7FJ|L7L7L7
// ....L7.F7||L7|.L7L7|
// .....|FJLJ|FJ|F7|.LJ
// ....FJL-7.||.||||...
// ....L---J.LJ.LJLJ...
// The above sketch has many random bits of ground, some of which are in the loop (I) and some of which are outside it (O):
// 
// OF----7F7F7F7F-7OOOO
// O|F--7||||||||FJOOOO
// O||OFJ||||||||L7OOOO
// FJL7L7LJLJ||LJIL-7OO
// L--JOL7IIILJS7F-7L7O
// OOOOF-JIIF7FJ|L7L7L7
// OOOOL7IF7||L7|IL7L7|
// OOOOO|FJLJ|FJ|F7|OLJ
// OOOOFJL-7O||O||||OOO
// OOOOL---JOLJOLJLJOOO
// In this larger example, 8 tiles are enclosed by the loop.
// 
// Any tile that isn't part of the main loop can count as being enclosed by the loop. Here's another example with many bits of junk pipe lying around that aren't connected to the main loop at all:
// 
// FF7FSF7F7F7F7F7F---7
// L|LJ||||||||||||F--J
// FL-7LJLJ||||||LJL-77
// F--JF--7||LJLJ7F7FJ-
// L---JF-JLJ.||-FJLJJ7
// |F|F-JF---7F7-L7L|7|
// |FFJF7L7F-JF7|JL---7
// 7-L-JL7||F7|L7F-7F7|
// L.L7LFJ|||||FJL7||LJ
// L7JLJL-JLJLJL--JLJ.L
// Here are just the tiles that are enclosed by the loop marked with I:
// 
// FF7FSF7F7F7F7F7F---7
// L|LJ||||||||||||F--J
// FL-7LJLJ||||||LJL-77
// F--JF--7||LJLJIF7FJ-
// L---JF-JLJIIIIFJLJJ7
// |F|F-JF---7IIIL7L|7|
// |FFJF7L7F-JF7IIL---7
// 7-L-JL7||F7|L7F-7F7|
// L.L7LFJ|||||FJL7||LJ
// L7JLJL-JLJLJL--JLJ.L
// In this last example, 10 tiles are enclosed by the loop.
// 
// Figure out whether you have time to search for the nest by calculating the area within the loop. How many tiles are enclosed by the loop?

struct Day10_2 : public Challenge
{
	bool IsClosedLoop(const std::vector<Tile>& maze)
	{
		if (maze.size() == 0) return false;

		Tile start = maze[0];
		Tile current = maze[0]; // start at 'S'
		for (const auto& tile : maze)
		{
			current += tile;
		}

		return start == current;
	}

	std::optional<char> GetTileType(Vec2 neighborPosition, const Input* input, bool countStars = false)
	{
		if (input->size() == 0) return {};

		size_t gridWidth = (*input)[0].size();
		size_t gridHeight = input->size();
		if (neighborPosition.x >= gridWidth || neighborPosition.y >= gridHeight)
		{
			// Out of bounds
			return {};
		}

		char type = (*input)[neighborPosition.y][neighborPosition.x];

		if (!countStars)
		{
			// Ignore '.' in the input
			if (type == '.') return {};
		}

		return type;
	}

	bool IsValidConnection(char currentType, Tile neighborOffset, char neighborType, bool forceConnectionToNeighbor = false)
	{
		// If this flag is set to true, we'll assume that the current tile connects to the neighbor. This is used
		// when calculating the starting tile, since we don't know what tile it is yet
		if (!forceConnectionToNeighbor)
		{
			const auto& validConnections = CharToTile.at(currentType);
			if (std::find(validConnections.begin(), validConnections.end(), neighborOffset) == validConnections.end())
			{
				// We don't connect to the neighbor
				return false;
			}
		}

		const auto& neighborConnections = CharToTile.at(neighborType);
		for (const auto& connection : neighborConnections)
		{
			// It's only a valid connection if the neighbor's type connects opposite to one of our connections
			// e.g. we connect East + South and the neighbor connects North + West. This is valid because we connect
			// South ([0, -1]) and the neighbor North ([0, 1]), meaning the sum of the two vectors is a zero vector
			if (neighborOffset + connection == Vec2(0, 0))
			{
				return true;
			}
		}

		return false;
	}

	void FloodFill_Recursive(Vec2 currentPosition, Input* input, char floodChar)
	{
		(*input)[currentPosition.y][currentPosition.x] = floodChar;

		std::vector<Vec2>* neighborPositions = new std::vector<Vec2>();
		neighborPositions->reserve(4);

		for (const auto& neighborOffset : NeighborKernel)
		{
			std::optional<char> neighborType = GetTileType(currentPosition + neighborOffset, input, true);
			if (neighborType.has_value())
			{
				char neighborVal = neighborType.value();
				if (neighborVal != '*' && neighborVal != 'O' && neighborVal != 'I')
				{
					Vec2 neighborPos = currentPosition + neighborOffset;
					(*input)[neighborPos.y][neighborPos.x] = floodChar;
					neighborPositions->push_back(neighborPos);
				}
			}
		}

		for (const auto& neighborPos : *neighborPositions)
		{
			FloodFill_Recursive(neighborPos, input, floodChar);
		}

		delete neighborPositions;
	}

	void FloodFill(Vec2 startingPosition, Input* input, char floodChar)
	{
		FloodFill_Recursive(startingPosition, input, floodChar);
	}

	void PrintInput(Input* input)
	{
		for (const auto& line : *input)
		{
			std::cout << line << std::endl;
		}
	}

	int Run(Input input)
	{
		// Represent indices into the input 'grid'
		Vec2 startingPoint(-1, -1);

		for (int i = 0; i < input.size(); i++)
		{
			const auto& line = input[i];

			startingPoint.x = line.find('S');
			if (startingPoint.x == std::string::npos)
			{
				continue;
			}

			// We found the starting point, cache Y index
			startingPoint.y = i;
			break;
		}

		// The maze contains a list of offsets starting from the starting point (not included in the maze). The index of the starting
		// point in the input is cached in startingPoint above
		std::vector<Tile> maze;
		maze.reserve(100);
		std::vector<Tile> validNeighbors;
		validNeighbors.reserve(4);

		// Calculate potential neighbors for starting point
		for (const Tile& neighbor : NeighborKernel)
		{
			auto neighborType = GetTileType(startingPoint + neighbor, &input);
			if (neighborType.has_value())
			{
				if (IsValidConnection('.', neighbor, neighborType.value(), true))
				{
					validNeighbors.push_back(neighbor);
				}

			}
		}

		// Run through every potential candidate for the starting point
		for (const Tile& startNeighborCandidate : validNeighbors)
		{
			// If the last tile in our closed maze is one of the neighbor candidates, then ignore it
			if (!maze.empty())
			{
				if (maze[maze.size() - 1] == Vec2(-startNeighborCandidate.x, -startNeighborCandidate.y))
				{
					continue;
				}
			}

			std::vector<Tile> currentNeighbors;
			currentNeighbors.reserve(4);
			currentNeighbors.push_back(startNeighborCandidate);

			maze.push_back(startNeighborCandidate);

			Vec2 previousPoint = startingPoint;
			Vec2 currentPoint = startingPoint + startNeighborCandidate;

			// Calculate the next neighbor and fill out the maze of tiles
			while (!IsClosedLoop(maze))
			{
				std::optional<char> currentType = GetTileType(currentPoint, &input);
				assert(currentType.has_value());

				// Calculate potential neighbors for current point
				currentNeighbors.clear();
				for (const Tile& neighbor : NeighborKernel)
				{
					// Skip counting the previous tile as a valid neighbor, since we just came from that tile
					if (currentPoint + neighbor == previousPoint) continue;

					auto neighborType = GetTileType(currentPoint + neighbor, &input);
					if (neighborType.has_value())
					{
						if (IsValidConnection(currentType.value(), neighbor, neighborType.value()))
						{
							currentNeighbors.push_back(neighbor);
						}
					}
				}

				if (currentNeighbors.empty())
				{
					// We know this candidate won't work because we reached a dead-end
					maze.clear();
					break;
				}

				// Consider branches??
				assert(currentNeighbors.size() == 1);

				Vec2 firstNeighbor = currentNeighbors[0];
				maze.push_back(firstNeighbor);

				// Update tracking variables
				previousPoint = currentPoint;
				currentPoint += firstNeighbor;
			}
		}

		// We now have a closed loop, turn every character in the loop into '*'
		Vec2 currentPosition = startingPoint;
		for (int i = 0; i < maze.size(); i++)
		{
			input[currentPosition.y][currentPosition.x] = '*';
			currentPosition += maze[i];
		}

		//std::cout << "\n\n Closed loop into '*'\n\n";
		//PrintInput(&input);

		// Everything inside the shape is to the [LEFT]
		// Everything outside the shape is to the [RIGHT]

		// Run around the loop and flood-fill either side, according to the above
		Vec2 prevPos = startingPoint;
		currentPosition = startingPoint;
		Vec2 prevTile = Vec2(0, 1);
		Tile heading = prevTile;
		for (int i = 0; i < maze.size(); i++)
		{
			Tile currTile = maze[i % maze.size()];
			Tile nextTile = maze[(i + 1) % maze.size()];

			if (!(currTile == nextTile))
			{
				// This is a corner tile, so consider the edges too
				// This is so hacky, but the way we'll do it is by considering
				// two headings on the corner (one before turning and one after turning)
				Tile headings[2] = { prevTile, currTile };
				for (int j = 0; j < 2; j++)
				{
					heading = headings[j];

					Tile left = Vec2(heading.y, -heading.x);
					std::optional<char> leftType = GetTileType(currentPosition + left, &input, true);
					if (leftType.has_value() && leftType.value() != '*')
					{
						FloodFill(currentPosition + left, &input, 'O');
					}

					Tile right = Vec2(-left.x, -left.y);
					std::optional<char> rightType = GetTileType(currentPosition + right, &input, true);
					if (rightType.has_value() && rightType.value() != '*')
					{
						FloodFill(currentPosition + right, &input, 'I');
					}
				}
			}
			else
			{
				heading = prevTile;

				// Calculate left/right based on heading
				// Heading changes according to where you have to turn for the next tile
				// heading (1, 0) then left (0, -1) / right (0, 1)
				// heading (-1, 0) then left (0, 1) / right (0, -1)
				// heading (0, 1) then left (1, 0) / right (-1, 0)
				// heading (0, -1) then left (-1, 0) / right (1, 0)
				Tile left = Vec2(heading.y, -heading.x);
				std::optional<char> leftType = GetTileType(currentPosition + left, &input, true);
				if (leftType.has_value() && leftType.value() != '*')
				{
					FloodFill(currentPosition + left, &input, 'O');
				}

				Tile right = Vec2(-left.x, -left.y);
				std::optional<char> rightType = GetTileType(currentPosition + right, &input, true);
				if (rightType.has_value() && rightType.value() != '*')
				{
					FloodFill(currentPosition + right, &input, 'I');
				}
			}

			prevPos = currentPosition;
			currentPosition += currTile;
			prevTile = currTile;
		}

		std::cout << "\n\n Flood fill using separating axis\n\n";
		PrintInput(&input);

		// Count up anything marked as 'I'
		size_t internalTiles = 0;
		for (auto& line : input)
		{
			for (auto& c : line)
			{
				if (c == 'I')
				{
					internalTiles++;
				}
			}
		}

		std::cout << "\n\nInternal tiles found: " << internalTiles << std::endl;;

		return internalTiles;
	}
};