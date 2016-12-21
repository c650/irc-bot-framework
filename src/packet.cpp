#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "./packet.hpp"
#include "./server.hpp"

namespace IRC {

	void Packet::reply(std::string msg) const {
		if (this->owner != nullptr)
			this->owner->privmsg(this->channel, msg);
	}

	bool Packet::_parse(char *buf) {

		if (strstr(buf, "!") == nullptr || strstr(buf, "@") == nullptr) {
			content = buf;
			return false;
		}

		std::cout << "Analyzing : " << buf;

		/* Get rid of \r\n */
		int n = strlen(buf);
		buf[n-1] = buf[n-2] = '\0';

		this->sender = strtok(buf, "!")+1; // gets user between : and !

		this->realname = strtok(nullptr, "@"); // skip over ~
		if (!this->realname.empty() && this->realname.front() == '~')
			this->realname.erase(this->realname.begin());

		this->hostname = strtok(nullptr, " ");

		this->type = strtok(nullptr, " ");
		if (this->type == "JOIN") {
			this->channel = strtok(nullptr, "\0") + 1;
			return true;
		}

		this->channel = strtok(nullptr, " ");
		if (channel.front() != '#')
			channel = sender;

		this->content = strtok(nullptr, "\0");

		if ( !this->content.empty() && ( this->type == "PRIVMSG" || this->type == "NOTICE"))
			this->content.erase(this->content.begin());

		if (this->type == "NICK")
			this->channel.erase(this->channel.begin()); // skip : in NICK messages

		std::cout << "\tPacket successfully analyzed.\n";
		return true;
	}
}
