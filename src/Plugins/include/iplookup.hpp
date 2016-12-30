#ifndef _IPLOOKUP_H
#define _IPLOOKUP_H

#include "../../IRCBot/include/command-interface.hpp"
#include "../../IRCBot/include/packet.hpp"

namespace IPLookup {

	/*
		Looks up a host and returns a string concatenation of City, Region, Country.

		@param host the host to look up. An IP address works best.
		@return the string concatenation of the location.
	*/
	std::string do_lookup(const std::string& host);

	class IPLookupCommand : public IRC::CommandInterface {

	public:

		IPLookupCommand() : IRC::CommandInterface("@iplookup ", "looks up IP address.", true) {}

		bool triggered(const IRC::Packet& p) const {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) const {
			p.reply(do_lookup(p.content.substr(this->trigger().length())));
		}
	};

};

#endif
