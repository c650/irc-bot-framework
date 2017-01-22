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

				if ( arg.find("-list") != std::string::npos) {

					std::string list_of_libs = "";
					this->bot_ptr->each_dynamic_plugin([&list_of_libs](const DynamicPluginLoading::DynamicPlugin * d){
						list_of_libs += d->get_name() + ", ";
					});
					if (!list_of_libs.empty()) {
						list_of_libs.erase(list_of_libs.end()-2, list_of_libs.end());
					} else {
						list_of_libs = "There are no dynamically loaded plugins at the moment.";
					}

					p.reply(list_of_libs);

					return;
				}

				command = command.substr(0, command.find(" "));

				if (command == "load") {

					try {
						this->bot_ptr->add_dynamic_command( arg );
					} catch (std::exception& e) {
						std::string ret = "Could not load: " + arg;
						p.reply( ret );
						std::cerr << ret << " " << e.what() << '\n';

						return;
					}

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

			std::string res = content.empty() ? do_help_general() : do_help_specific(content);

			p.owner->privmsg( p.sender , res );
		}
	  private:

		std::string do_help_general(void);
		std::string do_help_specific(const std::string& query);
	};

	class Statistics : protected IRC::CommandInterface {

	  public:
		Statistics(IRC::Bot* b) : CommandInterface("@stats", "does stats", b, true) {}

		void run(const IRC::Packet& p) {
			for (const auto& s : this->bot_ptr->get_stats()) {
				p.owner->privmsg(p.sender , s);
			}

			std::string tmp = "";
			this->bot_ptr->each_command([&tmp, &p](IRC::CommandInterface* command){
				tmp = command->get_stats();
				if (!tmp.empty())
					p.owner->privmsg(p.sender, tmp);
			});
		}

	};

	std::string Help::do_help_general(void) {

		std::string resp = "", tmp;
		this->bot_ptr->each_command([&resp, this, &tmp](IRC::CommandInterface* c) {
			if (c->trigger() != "@help" && (!c->requires_admin() || this->req_admin)) {
				tmp = c->trigger();
				strip_string(tmp);
				resp += tmp + ", ";
			}
		});

		/* guaranteed to have at least 2 chars because of the ", " added above.*/
		if (!resp.empty()) {
			resp.erase(resp.end() - 2, resp.end());
		} else {
			resp = "No commands installed!";
		}
		return resp;
	}

	std::string Help::do_help_specific(const std::string& query) {

		std::string ret = "[ERROR] Couldn't find " + query;
		this->bot_ptr->each_command([this, &query, &ret](IRC::CommandInterface* c) {
			if (c->trigger().find(query) != std::string::npos && (!c->requires_admin() || this->req_admin)) {
				ret = c->trigger() + " : " + c->desc();
				return;
			}
		});
		return ret;
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
