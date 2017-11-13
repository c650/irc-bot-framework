#include <regex>
#include <iostream>

int main(void) {

	const static std::regex PACKET_REGEX{"\r?:(\\S+) (\\w+)(\\s+[^:\\s]+)? :?(.+)?\r?\n?"};

	std::smatch match;

	std::string buf{":me!me@me.com PART #rickandmorty"};

	std::cout << std::regex_search(buf, match, PACKET_REGEX) << "\n";

	std::cout << match.str() << "\n";

	return 0;
}
