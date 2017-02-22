/*
	discourse-search.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/environment.hpp"
#include "./include/json.hpp"
#include "./include/http.hpp"

#include <algorithm>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

class DiscourseSearch : protected IRC::CommandInterface {

	struct DiscourseSite {
		std::string url, name, key, user;
	};

	/*static*/ std::vector<DiscourseSite> sites; // static doesn't work pretty with symbol resolution for some reason...

  public:

	DiscourseSearch() : IRC::CommandInterface("@search ", "searches a discourse. Ex. @search [Site] [User|Topic] [Search]") {
		if (sites.empty()) {
			try {
				nlohmann::json sites_json = ENVIRONMENT["discourse_sites"];
				for (auto& s : sites_json) {
					sites.push_back({s["url"].get<std::string>(),
					                 s["name"].get<std::string>(),
					                 s["api_key"].get<std::string>(),
					                 s["api_username"].get<std::string>()});
					std::transform(sites.back().name.begin(), sites.back().name.end(), sites.back().name.begin(), ::tolower);
				}

			} catch (std::exception& e) {
				std::cerr << "In DiscourseSearch(), couldn't parse sites: " << e.what() << '\n';
			}
		}
	}

	void run(const IRC::Packet& p) {

		p.reply("Searching as fast as I can...");

		// clean/validate query.
		std::vector<std::string> args;
		p.get_args(args);

		if (args.size() < 4) { // @search Site Type Query...
			std::cerr << "Had error in DiscourseSearch with this call: " << p.content << '\n';
			p.reply("Error. Invalid arguments. See @help search");
			return;
		}

		std::string site = args.at(1); // Site
		std::transform(site.begin(), site.end(), site.begin(), ::tolower);

		DiscourseSite* search_site = nullptr;
		for (auto& s : sites) {
			if (site == s.name) {
				search_site = &s;
				break;
			}
		}

		if (search_site == nullptr) {
			p.reply("Requested site: \"" + site + "\" is not currently searchable.");
			return;
		}

		std::string type = args.at(2); // Type
		std::transform(type.begin(), type.end(), type.begin(), ::tolower);

		if (type != "user" && type != "topic") {
			p.reply("Invalid type argument supplied: Choose Topic or User");
			return;
		}

		std::string query = "";
		for (size_t i = 3; i < args.size()-1; ++i) {
			query += args.at(i) + " ";
		}
		query = MyHTTP::uri_encode((type == "user" && !query.empty() ? "@" : "") + query + args.back());

		// perform.

		try {
			std::string response = "";
			MyHTTP::get( search_site->url + "/search.json?include_blurgs=false"
			            + "&api_key=" + search_site->key
			            + "&api_username=" + search_site->user
			            + "&q=" + query, response);
			nlohmann::json results = nlohmann::json::parse(response);

			results = results["topics"].at(0); // just keep topics.

			p.reply("First Result, I found: " + results["fancy_title"].get<std::string>() + " "
			        + search_site->url + "/t/" + std::to_string(results["id"].get<unsigned int>()));
			p.owner->privmsg(p.sender, "To view all search results -> " + search_site->url + "/search?q=" + query);

		} catch (std::exception& e) {
			std::cerr << "Something went wrong performing a discourse search on " << search_site->url << ": " << e.what() << '\n';
			p.reply("Failed to search. Sorry!");
		}
	}
};

extern "C" {

	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new DiscourseSearch);
	}

};
