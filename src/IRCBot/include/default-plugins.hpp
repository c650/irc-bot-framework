#ifndef _DEFAULT_PLUGINS_H
#define _DEFAULT_PLUGINS_H

#include "./command-interface.hpp"
#include "./bot.hpp"

#include <string>
#include <vector>

#include <iostream>

namespace DefaultPlugins {

	static void strip_string(std::string& s);

	static std::string do_help_general(const std::vector<const IRC::CommandInterface*>& cmds, const bool requires_admin);
	static std::string do_help_specific(const std::vector<const IRC::CommandInterface*>& cmds,
										const bool requires_admin, const std::string& query);

	class Help : protected IRC::CommandInterface {

	public:
		Help(IRC::Bot* b, bool ra) : CommandInterface("@help", "Helps users use bot.", b, ra) {}

		void run(const IRC::Packet& p) {
			size_t first_space = p.content.find(" ");
			std::string content = first_space != std::string::npos ? p.content.substr(first_space) : "";

			strip_string(content);

			auto cmds = this->bot_ptr->get_commands();
			std::string res = content.empty() ? do_help_general(cmds, this->req_admin) : do_help_specific(cmds, this->req_admin, content);

			p.owner->privmsg( p.sender , res );
		}
	};

	class Statistics : protected IRC::CommandInterface {

	public:
		Statistics(IRC::Bot* b) : CommandInterface("@stats", "does stats", b, true) {}

		void run(const IRC::Packet& p) {
			for (const auto& s : this->bot_ptr->get_stats()) {
				p.owner->privmsg(p.sender , s);
			}

			std::string tmp = "";
			for (auto command : this->bot_ptr->get_commands()) {
				tmp = command->get_stats();
				if (!tmp.empty())
					p.owner->privmsg(p.sender, tmp);
			}
		}

	};

	static std::string do_help_general(const std::vector<const IRC::CommandInterface*>& cmds, const bool requires_admin) {

		std::string resp = "";
		for (auto c : cmds) {
			if (c->trigger() != "@help" && requires_admin == c->requires_admin()) {
				resp += c->trigger() + ", ";
			}
		}

		if (!resp.empty()) {
			resp.pop_back();
		} else {
			resp = "No commands installed!";
		}
		return resp;
	}

	static std::string do_help_specific(const std::vector<const IRC::CommandInterface*>& cmds,
										const bool requires_admin, const std::string& query) {

		bool found = false;
		for (auto c : cmds) {
			if (c->trigger().find(query) != std::string::npos && (found = true) && requires_admin == c->requires_admin()) {
				return c->trigger() + " : " + c->desc();
			}
		}
		return found ? "" : "[ERROR] Couldn't find " + query;
	}

	static void strip_string(std::string& s) {
		size_t first = 0,
				last = s.length() - 1;

		while(first < s.length() && (s[first] == ' ' || s[first] == '\n' || s[first] == '\r'))
			first++;

		if (first == s.length()) {
			s = "";
			return;
		}

		while(last >= 0 && (s[last] == ' ' || s[last] == '\n' || s[last] == '\r'))
			last--;

		s = s.substr(first, last - first + 1);
	}

};

#endif
