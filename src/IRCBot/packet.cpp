#include <string>
#include <cstdio>
#include <cstdlib>

#include "./include/packet.hpp"
#include "./include/server.hpp"

namespace IRC {

	void Packet::reply(std::string msg) const {
		if (this->owner != nullptr)
			this->owner->privmsg(this->channel, msg);
	}

	bool Packet::_parse(std::string buf) {

		if (buf.find("!") == std::string::npos || buf.find("@") == std::string::npos) {
			content = buf;
			return false;
		}

		std::cout << "Analyzing : " << buf;

		/* Get rid of \r\n */
		buf.pop_back();
		buf.pop_back();

		size_t found = buf.find("!");
		this->sender = buf.substr(1, found - 1); // gets user between : and !
		buf = buf.erase(0, found+1);

		found = buf.find("@");
		this->realname = buf.substr(0, found); // skip over ~
		if (!this->realname.empty() && this->realname.front() == '~')
			this->realname.erase(this->realname.begin());
		buf = buf.erase(0, found+1);

		found = buf.find(" ");
		this->hostname = buf.substr(0, found);
		buf = buf.erase(0, found+1);

		found = buf.find(" ");
		this->type = _read_type(buf.substr(0, found));
		buf = buf.erase(0, found+1);

		if (this->type == PacketType::JOIN || this->type == PacketType::PART) {
			this->channel = buf.substr(this->type == PacketType::JOIN ? 1 : 0);
			return true;
		} else if (this->type == PacketType::NICK) {
			this->content = buf.substr(1); // +1 for :
			return true;
		}


		found = buf.find(" ");
		this->channel = buf.substr(0, found);
		buf = buf.erase(0, found+1);

		if (this->channel.front() != '#')
			this->channel = sender;

		this->content = buf;

		if ( !this->content.empty() && ( this->type == PacketType::PRIVMSG || this->type == PacketType::NOTICE))
			this->content.erase(this->content.begin());

		if (this->type == PacketType::NICK)
			this->channel.erase(this->channel.begin()); // skip : in NICK messages

		std::cout << "\tPacket successfully analyzed.\n";
		std::cout << "\tsender: " << this->sender
		          << "\n\trealname: " << this->realname
				  << "\n\thostname: " << this->hostname
				  << "\n\tchannel: " << this->channel
				  << "\n\tcontent: " << this->content << '\n';

				//   << "\n\ttype: " << this->type

		return true;
	}

	void Packet::get_args( std::vector<std::string>& vec ) const {
		std::cout << "Analyzing: " << content << "\n";

		size_t last = 0, next = 0;
		while(last < content.length() && (next = content.find(" ", last)) != std::string::npos) {
			vec.push_back(content.substr(last, next-last));

			last = next+1;
			while(last < content.length() && content[last] == ' ')
				last++;
		}
		vec.push_back(content.substr(last));
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
