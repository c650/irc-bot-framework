#ifndef _IRC_PACKET_H
#define _IRC_PACKET_H

#include <string>

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

		const Server *owner;

	private:

		bool valid;

	public:

		Packet() : valid(false) {}

		// Packet(std::string& pkt, const Server* o = nullptr)
		// 	: Packet(pkt.data(), o) {}

		Packet(char *buf, const Server* o = nullptr) : owner(o) {
			valid = _parse(buf);
		}

		bool is_valid() const {
			return valid;
		}

		void reply(std::string msg) const;

	private:

		bool _parse(char *buf);
	};
}

#endif
