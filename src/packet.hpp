#ifndef _IRC_PACKET_H
#define _IRC_PACKET_H

#include <string>
#include <vector>

#include "./types.hpp"
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

	private:

		bool valid;

	public:

		Packet() : valid(false), owner(nullptr) {}

		Packet(char *buf) : owner(nullptr) {
			valid = _parse(buf);
		}

		bool is_valid(void) const {
			return valid;
		}

		void reply(std::string msg) const;

		void get_args(std::vector<std::string>& vec) const;

	private:

		bool _parse(char *buf);
	};
}

#endif
