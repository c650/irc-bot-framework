/*
	nuller.cpp

	Command to remind people with the default "nullerXXXXX" name
	to pick a distinct nickname.
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

class NullerCommand : protected IRC::CommandInterface {

  public:

	NullerCommand() : IRC::CommandInterface("NULLER", "reminds nullerXXXXX's to change their nicks (not actually callable)") {}

	bool triggered(const IRC::Packet& p) {
		return p.type == IRC::Packet::PacketType::JOIN && p.sender.find("nuller") != std::string::npos;
	}

	void run(const IRC::Packet& p) {
		p.owner->privmsg(p.sender, "Hi there, " + p.sender + "! We're so glad you could join us! Please pick a" \
		                           " non-default, distinct nickname now!");
	}
};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new NullerCommand);
}
