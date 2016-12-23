#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "./include/packet.hpp"
#include "./include/server.hpp"

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
		if (this->type == "JOIN" || this->type == "PART") {
			this->channel = strtok(nullptr, "\0") + (this->type == "JOIN" ? 1 : 0);
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

}
