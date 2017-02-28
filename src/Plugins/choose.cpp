/*
	choose.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"

#include <string>
#include <random>

class ChooseCommand : protected IRC::CommandInterface {

	std::minstd_rand random_gen;

public:

	ChooseCommand() : CommandInterface("@choose ", "chooses a thing of comma-separated list.") {
		random_gen.seed(getpid());
	}

	bool triggered(const IRC::Packet& p) {
		if (p.content.length() < this->trigger_string.length())
			return false;

		std::string s = p.content.substr(0, this->trigger_string.length()-1);
		return p.type == IRC::Packet::PacketType::PRIVMSG && !std::isalpha(s[0]) && !std::isdigit(s[0]) && s.substr(1) == this->trigger_string.substr(1, s.length()-1);
	}

	void run(const IRC::Packet& p) {
		std::string choices_str = p.content.substr(this->trigger_string.length());

		std::vector<std::string> choices_vec;

		size_t last = 0, next = 0;
		while (last < choices_str.length() && (next = choices_str.find(",", last)) != std::string::npos) {
			choices_vec.push_back(choices_str.substr( last, next - last ) );

			last = next+1;
			while (last < choices_str.length() && choices_str[last] == ',')
				++last;
		}
		choices_vec.push_back(choices_str.substr(last));

		// std::cout << "choices_vec.size() = " << choices_vec.size() << '\n';

		if ( !choices_vec.empty() )
			p.reply(p.sender + ": " + choices_vec.at( random_gen() % choices_vec.size() ) );
	}

};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new ChooseCommand);
}
