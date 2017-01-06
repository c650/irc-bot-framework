#ifndef _COMMAND_INTERFACE_H
#define _COMMAND_INTERFACE_H

#include "./types.hpp"
#include "./packet.hpp"

#include <functional>
#include <string>

namespace IRC {

	class CommandInterface {

	  protected:

		std::string trigger_string, description;

		bool req_admin;

		Bot *bot_ptr;

	  public:

		CommandInterface(const std::string& ts, const std::string& d = "No Description Set.", Bot *b = nullptr, bool ra = false)
			: trigger_string(ts), description(d), req_admin(ra), bot_ptr(b) {}

		/*
			Inheritors will complete this function to
			determine whether or not a certain packet
			has the trigger for their command.

			@param p the packet to inspect.

			@return whether the packet is triggering.

			note: I'm making this have a default because most commands have
			a typical trigger behaviour.
		*/
		virtual bool triggered(const Packet& p) {
			return p.type == Packet::PacketType::PRIVMSG && p.content.substr(0, this->trigger_string.length()) == this->trigger_string;
		}

		virtual void run(const Packet& p) = 0;

		virtual std::string get_stats(void) {
			return trigger_string + ": N/A";
		}

		bool requires_admin(void) const {
			return req_admin;
		}

		std::string trigger(void) const {
			return trigger_string;
		}

		std::string desc(void) const {
			return description;
		}

		const Bot* bot(void) const {
			return bot_ptr;
		}

	};
};

#endif
