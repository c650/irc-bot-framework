#ifndef _IRC_PACKET_H
#define _IRC_PACKET_H

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring.h>

#include "./server.hpp"

namespace IRC {

	struct Packet {

		std::string sender,
		            realname,
					hostname,
					type,
					channel,

					content;

		Server *owner;

		bool valid;

	public:

		Packet(std::string& pkt)
			: this(pkt.data()) {}

		Packet(char *buf, Server* o = nullptr) {
			valid = _parse(buf);
			owner = o;
		}

		bool is_valid() {
			return valid;
		}

		bool reply(std::string msg);

	private:

		_parse(char *buf) {
			if (strstr(buf, "!") == nullptr || strstr(buf, "@") == nullptr) {
				return false;
			}

			/* Get rid of \r\n */
			int n = strlen(buf);
			buf[n-1] = buf[n-2] = '\0';

			this->sender = strtok(buf, "!")+1; // gets user between : and !

			this->realname = strtok(nullptr, "@"); // skip over ~
			if (!this->realname.empty() && this->realname.front() == '~')
				this->realname.erase(this->realname.begin());

			this->hostname = strtok(nullptr, " ");

			this->type = strtok(nullptr, " ");

			this->channel = strtok(nullptr, " ");

			this->content = strtok(nullptr, "\0");

			if ( !this->content.empty() && ( this->type == "PRIVMSG" || this->type == "NOTICE"))
				this->content.erase(this->content.begin());

			if (this->type == "NICK")
				this->channel.erase(this->channel.begin()); // skip : in NICK messages

			return true;
		}
	};

}

#endif
