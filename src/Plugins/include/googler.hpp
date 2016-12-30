#ifndef _GOOGLER_H
#define _GOOGLER_H

#include <string>
#include "./json.hpp"

#include "../../IRCBot/include/command-interface.hpp"
#include "../../IRCBot/include/packet.hpp"

namespace Googler {

	/*
		Performs a google search and then formats the results.

		@param query the query to google for. It's URI encoded inside the function,
			meaning that a human-readable query should be passed.
		@param num_res_to_show the number of google results desired.
		@res_vec a string vector in which the results are stored. "Title -> ShortenedLink"
	*/
	void do_google_search(const std::string& query,
	                      const size_t& num_res_to_show,
						  std::vector<std::string>& res_vec);

	class GoogleCommand : public IRC::CommandInterface {

	public:
		GoogleCommand() : CommandInterface("@google ", "Performs google search and returns shortened links.", true) {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {

			std::vector<std::string> res_vec;

			Googler::do_google_search(p.content.substr(this->trigger().length()), 2, res_vec);

			for (auto& res : res_vec) {
				p.reply(res);
			}

		}

	};
};

#endif
