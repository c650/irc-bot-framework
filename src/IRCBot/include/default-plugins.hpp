#ifndef _DEFAULT_PLUGINS_H
#define _DEFAULT_PLUGINS_H

#include "./command-interface.hpp"
#include "./bot.hpp"

#include "./dynamic-loading.hpp" /* for Loader */

#include <string>
#include <vector>

#include <iostream>
#include <stdexcept>

namespace DefaultPlugins {

	static void strip_string(std::string& s);

	class Loader : protected IRC::CommandInterface {

	public:

		Loader(IRC::Bot *b) : CommandInterface("@load", "load or unload a module", b, true) {}

		bool triggered(const IRC::Packet& p) {
			if (p.type == IRC::Packet::PacketType::PRIVMSG && p.content.length() >= this->trigger_string.length()) {
				std::string s = p.content.substr(1 , p.content.find(" ") - 1);
				return s == "load" || s == "unload";
			}
			return false;
		}

		void run(const IRC::Packet& p) {

			std::string command = "";
			try {
				command = p.content.substr(1);
				std::string arg = command.substr(command.find(" ") + 1);

				command = command.substr(0, command.find(" "));

				if (command == "load") {
					this->bot_ptr->add_dynamic_command( new DynamicPluginLoading::DynamicPlugin( arg ) );
				} else if (command == "unload") {
					this->bot_ptr->remove_dynamic_command( arg );
				}

				p.reply("Success.");

			} catch (std::exception& e) {
				std::string ret = "Could not load! Command passed: " + command;

				p.reply(ret);
				std::cerr << ret << " " << e.what() << '\n';
			}
		}

	};

	class Help : protected IRC::CommandInterface {

	  public:
		Help(IRC::Bot* b, bool ra) : CommandInterface("@help", "Helps users use bot.", b, ra) {}

		void run(const IRC::Packet& p) {
			size_t first_space = p.content.find(" ");
			std::string content = first_space != std::string::npos ? p.content.substr(first_space) : "";

			strip_string(content);

			auto cmds = this->bot_ptr->get_commands();
			std::string res = content.empty() ? do_help_general(cmds) : do_help_specific(cmds, content);

			p.owner->privmsg( p.sender , res );
		}
	  private:

		std::string do_help_general(const std::vector<const IRC::CommandInterface*>& cmds);
		std::string do_help_specific(const std::vector<const IRC::CommandInterface*>& cmds, const std::string& query);
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

	std::string Help::do_help_general(const std::vector<const IRC::CommandInterface*>& cmds) {

		std::string resp = "", tmp;
		for (auto c : cmds) {
			if (c->trigger() != "@help" && this->req_admin == c->requires_admin()) {
				tmp = c->trigger();
				strip_string(tmp);
				resp += tmp + ", ";
			}
		}

		/* guaranteed to have at least 2 chars because of the ", " added above.*/
		if (!resp.empty()) {
			resp.erase(resp.end() - 2, resp.end());
		} else {
			resp = "No commands installed!";
		}
		return resp;
	}

	std::string Help::do_help_specific(const std::vector<const IRC::CommandInterface*>& cmds, const std::string& query) {

		bool found = false;
		for (auto c : cmds) {
			if (c->trigger().find(query) != std::string::npos && (found = true) && this->req_admin == c->requires_admin()) {
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
