
#include <vector>
#include <string>

struct Challenge
{
	virtual int Run(std::vector<std::string> input) = 0;
};