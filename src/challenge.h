
#include <vector>
#include <string>

struct Challenge
{
	typedef std::vector<std::string> Input;
	virtual int Run(Input input) = 0;
};