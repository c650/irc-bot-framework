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

		/*
			Default constructor.
			By default, a packet is invalid and has no owner.
		*/
		Packet() : valid(false), owner(nullptr) {}

		/*
			@param buf the buffer to parse into a Packet. (see the declared member variables)
		*/
		Packet(std::string buf) : owner(nullptr) {
			valid = _parse(buf);
		}

		/*
			@returns whether or not a packet is valid.
		*/
		bool is_valid(void) const {
			return valid;
		}

		/*
			Shortcut function to send a privmsg back to the appropriate endpoint
			on the server pointed to by `owner`.

			@param msg the message to send.
		*/
		void reply(std::string msg) const;

		/*
			Splits a packet's content along spaces.

			@param vec the vector of strings in which the parsed args
				will be saved.
		*/
		void get_args(std::vector<std::string>& vec) const;

	private:

		/*
			Parses the buffer into relevant sections.

			@param buf the buffer to parse.
		*/
		bool _parse(std::string buf);
	};
}

#endif
