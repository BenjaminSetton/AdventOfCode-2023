// --- Day 8: Haunted Wasteland ---
// You're still riding a camel across Desert Island when you spot a sandstorm quickly approaching. When you turn to warn the Elf, she disappears before your eyes! To be fair, she had just finished warning you about ghosts a few minutes ago.
// 
// One of the camel's pouches is labeled "maps" - sure enough, it's full of documents (your puzzle input) about how to navigate the desert. At least, you're pretty sure that's what they are; one of the documents contains a list of left/right instructions, and the rest of the documents seem to describe some kind of network of labeled nodes.
// 
// It seems like you're meant to use the left/right instructions to navigate the network. Perhaps if you have the camel follow the same instructions, you can escape the haunted wasteland!
// 
// After examining the maps for a bit, two nodes stick out: AAA and ZZZ. You feel like AAA is where you are now, and you have to follow the left/right instructions until you reach ZZZ.
// 
// This format defines each node of the network individually. For example:
// 
// RL
// 
// AAA = (BBB, CCC)
// BBB = (DDD, EEE)
// CCC = (ZZZ, GGG)
// DDD = (DDD, DDD)
// EEE = (EEE, EEE)
// GGG = (GGG, GGG)
// ZZZ = (ZZZ, ZZZ)
// Starting with AAA, you need to look up the next element based on the next left/right instruction in your input. In this example, start with AAA and go right (R) by choosing the right element of AAA, CCC. Then, L means to choose the left element of CCC, ZZZ. By following the left/right instructions, you reach ZZZ in 2 steps.
// 
// Of course, you might not find ZZZ right away. If you run out of left/right instructions, repeat the whole sequence of instructions as necessary: RL really means RLRLRLRLRLRLRLRL... and so on. For example, here is a situation that takes 6 steps to reach ZZZ:
// 
// LLR
// 
// AAA = (BBB, BBB)
// BBB = (AAA, ZZZ)
// ZZZ = (ZZZ, ZZZ)
// Starting at AAA, follow the left/right instructions. How many steps are required to reach ZZZ?

#include "../challenge.h"

#include <string>
#include <optional>
#include <numeric>
#include <unordered_map>

typedef std::string Node;
typedef std::unordered_map<Node, std::pair<Node, Node>> Map;
const Node END_NODE = "ZZZ";

struct Day8_1 : public Challenge
{

	void ParseLine(std::string line, Map& out_map)
	{
		Node currNode = line.substr(0, 3);
		Node left = line.substr(line.find('(') + 1, 3);
		Node right = line.substr(line.find(')') - 3, 3);
		out_map.insert({ currNode, { left, right } });
	}

	int Run(Input input)
	{
		std::string steps = input[0];

		// Construct map
		Map nodeMap;
		for (int i = 2; i < input.size(); i++)
		{
			ParseLine(input[i], nodeMap);
		}

		// Assuming it takes a finite number of steps to reach 'ZZZ'...
		Node currNode = "AAA";
		int numSteps = 0;
		while (currNode != END_NODE)
		{
			for (const auto& step : steps)
			{
				if (step == 'L')
				{
					currNode = nodeMap.at(currNode).first;
				}
				else
				{
					currNode = nodeMap.at(currNode).second;
				}

				numSteps++;

				if (currNode == END_NODE)
				{
					return numSteps;
				}

			}
		}

		return numSteps;
	}
};

// --- Part Two ---
// The sandstorm is upon you and you aren't any closer to escaping the wasteland. You had the camel follow the instructions, but you've barely left your starting position. It's going to take significantly more steps to escape!
// 
// What if the map isn't for people - what if the map is for ghosts? Are ghosts even bound by the laws of spacetime? Only one way to find out.
// 
// After examining the maps a bit longer, your attention is drawn to a curious fact: the number of nodes with names ending in A is equal to the number ending in Z! If you were a ghost, you'd probably just start at every node that ends with A and follow all of the paths at the same time until they all simultaneously end up at nodes that end with Z.
// 
// For example:
// 
// LR
// 
// 11A = (11B, XXX)
// 11B = (XXX, 11Z)
// 11Z = (11B, XXX)
// 22A = (22B, XXX)
// 22B = (22C, 22C)
// 22C = (22Z, 22Z)
// 22Z = (22B, 22B)
// XXX = (XXX, XXX)
// Here, there are two starting nodes, 11A and 22A (because they both end with A). As you follow each left/right instruction, use that instruction to simultaneously navigate away from both nodes you're currently on. Repeat this process until all of the nodes you're currently on end with Z. (If only some of the nodes you're on end with Z, they act like any other node and you continue as normal.) In this example, you would proceed as follows:
// 
// Step 0: You are at 11A and 22A.
// Step 1: You choose all of the left paths, leading you to 11B and 22B.
// Step 2: You choose all of the right paths, leading you to 11Z and 22C.
// Step 3: You choose all of the left paths, leading you to 11B and 22Z.
// Step 4: You choose all of the right paths, leading you to 11Z and 22B.
// Step 5: You choose all of the left paths, leading you to 11B and 22C.
// Step 6: You choose all of the right paths, leading you to 11Z and 22Z.
// So, in this example, you end up entirely on nodes that end in Z after 6 steps.
// 
// Simultaneously start on every node that ends with A. How many steps does it take before you're only on nodes that end with Z?

struct Node2
{
	size_t id, left, right;
	bool isStart = false;
	bool isEnd = false;
};

struct Day8_2 : public Challenge
{
	std::unordered_map<size_t, std::string> nodeIDToString;
	std::unordered_map<std::string, size_t> stringToNodeID;
	std::vector<Node2> mainContainer;

	// Optionally returns a node, as long as it's a starting node
	std::optional<size_t> ParseNode(std::string line)
	{
		std::string nodeStr = line.substr(0, 3);

		int nodeID = mainContainer.size();

		// main container
		Node2 newNode;
		newNode.id = nodeID;
		newNode.isStart = IsStartingNode(nodeStr);
		newNode.isEnd = IsEndingNode(nodeStr);
		mainContainer.push_back(newNode);

		// node ID to string
		nodeIDToString.insert({ nodeID, nodeStr });

		// string to node ID
		stringToNodeID.insert({ nodeStr, nodeID });

		if (newNode.isStart)
		{
			return nodeID;
		}

		return {};
	}

	void ParseNeighbors(std::string line, Node2& node)
	{
		// nodeNeighbors
		std::string left = line.substr(line.find('(') + 1, 3);
		std::string right = line.substr(line.find(')') - 3, 3);

		node.left = stringToNodeID.at(left);
		node.right = stringToNodeID.at(right);
	}

	bool IsStartingNode(const std::string& nodeStr)
	{
		return nodeStr[2] == 'A';
	}

	bool IsEndingNode(const std::string& nodeStr)
	{
		return nodeStr[2] == 'Z';
	}

	int Run(Input input)
	{
		std::string steps = input[0];

		// First iteration, construct main container and nodeToString
		std::vector<size_t> startingNodes;
		for (int i = 2; i < input.size(); i++)
		{
			const auto& line = input[i];
			auto opt = ParseNode(line);
			if (opt.has_value())
			{
				startingNodes.push_back(opt.value());
			}
		}

		// Second iteration, construct neighbors
		for (int i = 2; i < input.size(); i++)
		{
			const auto& line = input[i];
			size_t nodeID = i - 2;
			ParseNeighbors(line, mainContainer[nodeID]);
		}

		std::vector<size_t> currentNodeIDs = startingNodes;
		std::vector<size_t> numStepsPerNode;
		numStepsPerNode.resize(startingNodes.size());

		bool finished = false;
		for (int i = 0; i < currentNodeIDs.size(); i++)
		{
			size_t& currNodeID = currentNodeIDs[i];
			size_t stepsTaken = 0;
			while (!mainContainer[currNodeID].isEnd)
			{
				finished = false;
				for (const auto& step : steps)
				{
					Node2* currNode = &mainContainer[currNodeID];

					bool isStepLeft = step == 'L';
					int newID = currNode->left * static_cast<size_t>(isStepLeft) + currNode->right * static_cast<size_t>(!isStepLeft);
					currNodeID = newID;

					stepsTaken++;

					std::cout << nodeIDToString[currNodeID] << " ";

					if (mainContainer[currNodeID].isEnd)
					{
						finished = true;
						break;
					}
				}

				std::cout << std::endl;

				if (finished)
				{
					std::cout << "Node " << nodeIDToString.at(currNodeID) << " finished in " << stepsTaken << " steps!" << std::endl;
					numStepsPerNode[i] = stepsTaken;
					break;
				}
			}
		}

		// Calculate the result (least-common-denominator between all the minimum steps)
		size_t result = std::accumulate(numStepsPerNode.begin(), numStepsPerNode.end(), 1ull, std::lcm<size_t, size_t>);

		std::cout << "Result (size_t): " << result << std::endl;

		return result;
	}
};