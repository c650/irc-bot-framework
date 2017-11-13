#include <string>
#include <cstdio>
#include <cstdlib>

#include "./include/packet.hpp"
#include "./include/server.hpp"
#include "./include/user.hpp"

#include <regex>

namespace IRC {

	void Packet::reply(const std::string& msg) const {
		if (this->owner != nullptr)
			this->owner->privmsg(this->channel, msg);
	}

	bool Packet::_parse(const std::string& buf) {

		const static std::regex PACKET_REGEX{"\r?:(\\S+) (\\w+)(?:\\s+)?([^:\\s]+)? :?(.+)?\r?\n?"};

		if (buf.find("!") == std::string::npos || buf.find("@") == std::string::npos) {
			this->content = buf;
			return false;
		}

		std::cout << "Analyzing : " << buf;

		std::smatch match;
		if (!std::regex_search(buf, match, PACKET_REGEX)) {
			std::cout << buf << " does not match.\n";
			return false;
		}

		try {
			this->sender_user_object = User(match[1].str());

			this->sender = this->sender_user_object.get_nick();
			this->realname = this->sender_user_object.get_realname();
			this->hostname = this->sender_user_object.get_hostname();
		} catch (...) {
			this->content = buf;
			return false;
		}

		this->type = _read_type(match[2].str());

		this->channel = match[3].str();
		this->content = match[4].str();

		if (this->type == PacketType::JOIN || (this->type == PacketType::PART && this->channel.empty())) {
			this->channel = match[4].str();
		}

		if (this->channel.front() != '#')
			this->channel = sender;

		std::cout << "\tPacket successfully analyzed.\n"
		          << "\tsender: " << this->sender
		          << "\n\trealname: " << this->realname
				  << "\n\thostname: " << this->hostname
				  << "\n\tchannel: " << this->channel
				  << "\n\tcontent: " << this->content << '\n';

		return true;
	}

	Packet::PacketType Packet::_read_type(const std::string& t) {
		if (t == "PRIVMSG")
			return PacketType::PRIVMSG;
		if (t == "JOIN")
			return PacketType::JOIN;
		if (t == "PART")
			return PacketType::PART;
		if (t == "KICK")
			return PacketType::KICK;
		if (t == "INVITE")
			return PacketType::INVITE;
		if (t == "LIST")
			return PacketType::LIST;
		if (t == "MODE")
			return PacketType::MODE;
		if (t == "NICK")
			return PacketType::NICK;
		if (t == "PING")
			return PacketType::PING;
		if (t == "PONG")
			return PacketType::PONG;
		if (t == "QUIT")
			return PacketType::QUIT;
		if (t == "WHO")
			return PacketType::WHO;
		if (t == "WHOIS")
			return PacketType::WHOIS;
		if (t == "WHOWAS")
			return PacketType::WHOWAS;

		return PacketType::OTHER;
	}

}
