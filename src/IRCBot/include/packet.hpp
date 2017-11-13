#ifndef _IRC_PACKET_H
#define _IRC_PACKET_H

#include <string>
#include <vector>

#include "./types.hpp"
#include "./server.hpp"
#include "./user.hpp"

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

		User sender_user_object;

		Server *owner;

	private:

		bool valid;

	public:

		/*
			Default constructor.
			By default, a packet is invalid and has no owner.
		*/
		Packet() : Packet("") {}

		/*
			@param buf the buffer to parse into a Packet. (see the declared member variables)
		*/
		Packet(const std::string& buf) : Packet(buf, nullptr) {}

		Packet(const std::string& buf, Server *o) : owner(o) {
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
		void reply(const std::string& msg) const;

	private:

		/*
			Parses the buffer into relevant sections.

			@param buf the buffer to parse.
		*/
		bool _parse(const std::string& buf);

		/*
			Determines type from string

			@param t the type as a string.
		*/
		PacketType _read_type(const std::string& t);
	};
}

#endif
