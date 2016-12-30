#ifndef _IRC_PACKET_H
#define _IRC_PACKET_H

#include <string>
#include <vector>

#include "./types.hpp"
#include "./server.hpp"

namespace IRC {

	struct Packet {

		enum class PacketType {
			PRIVMSG,
			JOIN,
			PART,
			KICK,
			INVITE,
			LIST,
			MODE,
			NICK,
			PING,
			PONG,
			QUIT,
			WHO,
			WHOIS,
			WHOWAS,
			NOTICE,
			OTHER
		};

		std::string sender,
		            realname,
					hostname,
					channel,
					content;
		PacketType type;

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

		/*
			Determines type from string

			@param t the type as a string.
		*/
		PacketType _read_type(const std::string& t);
	};
}

#endif
