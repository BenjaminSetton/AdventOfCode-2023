// --- Day 7: Camel Cards ---
// Your all-expenses-paid trip turns out to be a one-way, five-minute ride in an airship. (At least it's a cool airship!) It drops you off at the edge of a vast desert and descends back to Island Island.
// 
// "Did you bring the parts?"
// 
// You turn around to see an Elf completely covered in white clothing, wearing goggles, and riding a large camel.
// 
// "Did you bring the parts?" she asks again, louder this time. You aren't sure what parts she's looking for; you're here to figure out why the sand stopped.
// 
// "The parts! For the sand, yes! Come with me; I will show you." She beckons you onto the camel.
// 
// After riding a bit across the sands of Desert Island, you can see what look like very large rocks covering half of the horizon. The Elf explains that the rocks are all along the part of Desert Island that is directly above Island Island, making it hard to even get there. Normally, they use big machines to move the rocks and filter the sand, but the machines have broken down because Desert Island recently stopped receiving the parts they need to fix the machines.
// 
// You've already assumed it'll be your job to figure out why the parts stopped when she asks if you can help. You agree automatically.
// 
// Because the journey will take a few days, she offers to teach you the game of Camel Cards. Camel Cards is sort of similar to poker except it's designed to be easier to play while riding a camel.
// 
// In Camel Cards, you get a list of hands, and your goal is to order them based on the strength of each hand. A hand consists of five cards labeled one of A, K, Q, J, T, 9, 8, 7, 6, 5, 4, 3, or 2. The relative strength of each card follows this order, where A is the highest and 2 is the lowest.
// 
// Every hand is exactly one type. From strongest to weakest, they are:
// 
// Five of a kind, where all five cards have the same label: AAAAA
// Four of a kind, where four cards have the same label and one card has a different label: AA8AA
// Full house, where three cards have the same label, and the remaining two cards share a different label: 23332
// Three of a kind, where three cards have the same label, and the remaining two cards are each different from any other card in the hand: TTT98
// Two pair, where two cards share one label, two other cards share a second label, and the remaining card has a third label: 23432
// One pair, where two cards share one label, and the other three cards have a different label from the pair and each other: A23A4
// High card, where all cards' labels are distinct: 23456
// Hands are primarily ordered based on type; for example, every full house is stronger than any three of a kind.
// 
// If two hands have the same type, a second ordering rule takes effect. Start by comparing the first card in each hand. If these cards are different, the hand with the stronger first card is considered stronger. If the first card in each hand have the same label, however, then move on to considering the second card in each hand. If they differ, the hand with the higher second card wins; otherwise, continue with the third card in each hand, then the fourth, then the fifth.
// 
// So, 33332 and 2AAAA are both four of a kind hands, but 33332 is stronger because its first card is stronger. Similarly, 77888 and 77788 are both a full house, but 77888 is stronger because its third card is stronger (and both hands have the same first and second card).
// 
// To play Camel Cards, you are given a list of hands and their corresponding bid (your puzzle input). For example:
// 
// 32T3K 765
// T55J5 684
// KK677 28
// KTJJT 220
// QQQJA 483
// This example shows five hands; each hand is followed by its bid amount. Each hand wins an amount equal to its bid multiplied by its rank, where the weakest hand gets rank 1, the second-weakest hand gets rank 2, and so on up to the strongest hand. Because there are five hands in this example, the strongest hand will have rank 5 and its bid will be multiplied by 5.
// 
// So, the first step is to put the hands in order of strength:
// 
// 32T3K is the only one pair and the other hands are all a stronger type, so it gets rank 1.
// KK677 and KTJJT are both two pair. Their first cards both have the same label, but the second card of KK677 is stronger (K vs T), so KTJJT gets rank 2 and KK677 gets rank 3.
// T55J5 and QQQJA are both three of a kind. QQQJA has a stronger first card, so it gets rank 5 and T55J5 gets rank 4.
// Now, you can determine the total winnings of this set of hands by adding up the result of multiplying each hand's bid with its rank (765 * 1 + 220 * 2 + 28 * 3 + 684 * 4 + 483 * 5). So the total winnings in this example are 6440.
// 
// Find the rank of every hand in your set. What are the total winnings?

#include "../challenge.h"

#include <assert.h>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

typedef std::string Hand;
typedef std::unordered_map<char, int> Tally;

void TallyHand(Hand hand, Tally& out_tally)
{
	for (const auto& card : hand)
	{
		if (out_tally.find(card) != out_tally.end())
		{
			out_tally[card] = out_tally[card] + 1;
		}
		else
		{
			out_tally.insert({ card, 1 });
		}
	}
}

bool FiveOfAKind(Tally tally)
{
	return tally.size() == 1; // All 5 cards are equal
}

bool FourOfAKind(Tally tally)
{
	return (tally.size() == 2) && 
		(std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 1; }) &&
		 std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 4; }));
}

bool FullHouse(Tally tally)
{
	return (tally.size() == 2) &&
		(std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 3; }) &&
		 std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 2; }));
}

bool ThreeOfAKind(Tally tally)
{
	return (tally.size() == 3) &&
		(std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 3; }) &&
		 std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 1; }));
}

bool TwoPair(Tally tally)
{
	return (tally.size() == 3) &&
		(std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 2; }) &&
		 std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 1; }));
}

bool OnePair(Tally tally)
{
	return (tally.size() == 4) &&
		(std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 2; }) &&
		 std::any_of(tally.begin(), tally.end(), [](auto iter) { return iter.second == 1; }));
}

bool HighCard(Tally tally)
{
	return tally.size() == 5;
}

const std::unordered_map<char, int> Cards =
{
	{ 'J', 1  },
	{ '2', 2  },
	{ '3', 3  },
	{ '4', 4  },
	{ '5', 5  },
	{ '6', 6  },
	{ '7', 7  },
	{ '8', 8  },
	{ '9', 9  },
	{ 'T', 10 },
	{ 'Q', 11 },
	{ 'K', 12 },
	{ 'A', 13 }
};

const std::vector<std::function<bool(Tally)>> HandTypes =
{
	HighCard,
	OnePair,
	TwoPair,
	ThreeOfAKind,
	FullHouse,
	FourOfAKind,
	FiveOfAKind
};

// DEBUG
static const std::vector<std::string> HandTypeNames =
{
	"HighCard",
	"OnePair",
	"TwoPair",
	"ThreeOfAKind",
	"FullHouse",
	"FourOfAKind",
	"FiveOfAKind"
};

struct Day7_1 : public Challenge
{
	int Run(Input input)
	{
		std::unordered_map<Hand, int> handToBidList;

		for (const auto& line : input)
		{
			int spaceIndex = line.find(' ');
			Hand hand = line.substr(0, spaceIndex);
			int bid = std::stoi(line.substr(spaceIndex + 1, line.size()));
			handToBidList.insert({ hand, bid });
		}

		std::vector<std::vector<Hand>> rankingFirstRule;
		rankingFirstRule.resize(HandTypes.size());

		for (const auto& iter : handToBidList)
		{
			const Hand hand = iter.first;

			Tally tally;
			TallyHand(hand, tally);

			for (int i = 0; i < HandTypes.size(); i++)
			{
				auto func = HandTypes[i];
				if (func(tally))
				{
					// This hand passed the first test, try to rank it
					std::cout << "Hand '" << hand << "' is " << HandTypeNames[i] << std::endl;
					auto& vec = rankingFirstRule[i];
					vec.push_back(hand);
					break;
				}
			}
		}

		std::vector<Hand> rankingSecondRule;
		rankingSecondRule.reserve(handToBidList.size());

		for (auto& rank : rankingFirstRule)
		{
			if (rank.size() == 0) continue;
			else if (rank.size() == 1)
			{
				rankingSecondRule.push_back(rank[0]);
			}
			else
			{
				// If multiple cards have the same rank, sort them using second ordering rule
				std::sort(rank.begin(), rank.end(), [](Hand a, Hand b) {
					int i = 0;
					while (i < 5 && (a[i] == b[i])) { i++; }

					char aC = a[i];
					char bC = b[i];
					return Cards.at(aC) < Cards.at(bC);

					});

				// Now push back the ordered hands
				for (const auto& hand : rank)
				{
					rankingSecondRule.push_back(hand);
				}
			}
		}

		// Finally calculate the total winnings
		uint64_t winnings = 0;
		for (uint64_t i = 0; i < rankingSecondRule.size(); i++)
		{
			winnings += (i + 1) * static_cast<uint64_t>(handToBidList[rankingSecondRule[i]]);
		}

		std::cout << "Real result: " << winnings << std::endl;

		return winnings;
	}
};

// --- Part Two ---
// To make things a little more interesting, the Elf introduces one additional rule. Now, J cards are jokers - wildcards that can act like whatever card would make the hand the strongest type possible.
// 
// To balance this, J cards are now the weakest individual cards, weaker even than 2. The other cards stay in the same order: A, K, Q, T, 9, 8, 7, 6, 5, 4, 3, 2, J.
// 
// J cards can pretend to be whatever card is best for the purpose of determining hand type; for example, QJJQ2 is now considered four of a kind. However, for the purpose of breaking ties between two hands of the same type, J is always treated as J, not the card it's pretending to be: JKKK2 is weaker than QQQQ2 because J is weaker than Q.
// 
// Now, the above example goes very differently:
// 
// 32T3K 765
// T55J5 684
// KK677 28
// KTJJT 220
// QQQJA 483
// 32T3K is still the only one pair; it doesn't contain any jokers, so its strength doesn't increase.
// KK677 is now the only two pair, making it the second-weakest hand.
// T55J5, KTJJT, and QQQJA are now all four of a kind! T55J5 gets rank 3, QQQJA gets rank 4, and KTJJT gets rank 5.
// With the new joker rule, the total winnings in this example are 5905.
// 
// Using the new joker rule, find the rank of every hand in your set. What are the new total winnings?

struct Day7_2 : public Challenge
{
	int Run(Input input)
	{
		std::unordered_map<Hand, int> handToBidList;

		for (const auto& line : input)
		{
			int spaceIndex = line.find(' ');
			Hand hand = line.substr(0, spaceIndex);
			int bid = std::stoi(line.substr(spaceIndex + 1, line.size()));
			handToBidList.insert({ hand, bid });
		}

		std::vector<std::vector<Hand>> rankingFirstRule;
		rankingFirstRule.resize(HandTypes.size());

		for (const auto& iter : handToBidList)
		{
			const Hand hand = iter.first;

			Tally tally;
			TallyHand(hand, tally);

			// I'm assuming it's ALWAYS optimal to convert ALL Joker cards into the same exact card, as opposed
			// to making them all different cards or something. What we'll do is remove Jokers from the tally as
			// individual cards, and instead add their counts to other cards already present in the hand
			if (tally.find('J') != tally.end())
			{
				int jokerCount = tally.at('J');
				Tally modifiedTally = tally;
				modifiedTally.erase(modifiedTally.find('J'));

				// If we have a joker FiveOfAKind then we can't run this logic because the main assumption here
				// is that we have cards other than the joker(s)
				int bestRank = -1;

				if (jokerCount == 5)
				{
					bestRank = HandTypes.size() - 1;
				}
				else
				{
					// Find the best rank by pretending the jokers are now other cards
					for (auto& iter : modifiedTally)
					{
						iter.second += jokerCount;

						bool foundType = false; // DEBUG
						for (int j = 0; j < HandTypes.size(); j++)
						{
							auto func = HandTypes[j];
							if (func(modifiedTally))
							{
								// This hand passed the first test, try to rank it
								std::cout << "\t" << "[JOKER] Hand '" << hand << "' is " << HandTypeNames[j] << std::endl;
								bestRank = std::max(bestRank, j);
								foundType = true;
								break;
							}
						}

						assert(foundType && "We didn't find a type for this card?");

						iter.second -= jokerCount;
					}
				}

				// Push back the hand only to the best rank
				std::cout << "Found best joker iteration for hand '" << hand << "' to be " << HandTypeNames[bestRank] << std::endl;
				auto& vec = rankingFirstRule[bestRank];
				vec.push_back(hand);
			}
			else // proceed as usual
			{
				for (int i = 0; i < HandTypes.size(); i++)
				{
					auto func = HandTypes[i];
					if (func(tally))
					{
						// This hand passed the first test, try to rank it
						std::cout << "Hand '" << hand << "' is " << HandTypeNames[i] << std::endl;
						auto& vec = rankingFirstRule[i];
						vec.push_back(hand);
						break;
					}
				}
			}

		}

		std::vector<Hand> rankingSecondRule;
		rankingSecondRule.reserve(handToBidList.size());

		for (auto& rank : rankingFirstRule)
		{
			if (rank.size() == 0) continue;
			else if (rank.size() == 1)
			{
				rankingSecondRule.push_back(rank[0]);
			}
			else
			{
				// If multiple cards have the same rank, sort them using second ordering rule
				std::sort(rank.begin(), rank.end(), [](Hand a, Hand b) {
					int i = 0;
					while (i < 5 && (a[i] == b[i])) { i++; }

					char aC = a[i];
					char bC = b[i];
					return Cards.at(aC) < Cards.at(bC);

					});

				// Now push back the ordered hands
				for (const auto& hand : rank)
				{
					rankingSecondRule.push_back(hand);
				}
			}
		}

		// Finally calculate the total winnings
		uint64_t winnings = 0;
		for (uint64_t i = 0; i < rankingSecondRule.size(); i++)
		{
			winnings += (i + 1) * static_cast<uint64_t>(handToBidList[rankingSecondRule[i]]);
		}

		std::cout << "Real result: " << winnings << std::endl;

		return winnings;
	}
};