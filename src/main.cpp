#include "./include/environment.hpp"

#include "./include/json.hpp" /* from https://github.com/nlohmann/json */
#include "./include/googler.hpp"
#include "./include/random-line-stream.hpp"
#include "./include/iplookup.hpp"

#include "./include/bot.hpp"
#include "./include/packet.hpp"

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <curl/curl.h>
#include <cstring>
#include <random>

#define REQUIRES_ADMIN_PERMS true

#define DEFAULT_CONFIG_PATH "./config.json"

nlohmann::json ENVIRONMENT; /* Global Environment Variable. */

RandomLineStream babbler;

int main(void) {

	/* Load the environment */
	try {
		std::fstream fs(DEFAULT_CONFIG_PATH, std::fstream::in);
		fs >> ENVIRONMENT;
		fs.close();
	} catch(...) {
		std::cerr << "Couldn't open " << DEFAULT_CONFIG_PATH << '\n';
		return 1;
	}


	/* Simple startup. Initialize a bot with a nick, password, and admin. */
	IRC::Bot b(ENVIRONMENT["bot"]["nick"].get<std::string>(), /* Nickname */
	           ENVIRONMENT["bot"]["pass"].get<std::string>(), /* Password, can be "" */
			   ENVIRONMENT["bot"]["admins"].get<std::vector<std::string>>()); /* List of admins. */
	// The program SHOULD crash if the above 3 lines don't work.

	babbler.load_file(ENVIRONMENT["babble"]["filepath"].get<std::string>());

	/* And here are some actions/commands you can do! */
	b.on_privmsg("@sayhi", [](const IRC::Packet& packet){
		packet.reply("Hello!");
	}, "says hello");

	b.on_privmsg("@speak ", [](const IRC::Packet& packet){

		std::string arguments = packet.content.substr(7); // everything after "@speak "

		size_t first_space_idx = arguments.find(" ");
		if (first_space_idx == std::string::npos)
			return;
		std::string chan = arguments.substr(0, first_space_idx);
		std::string msg = arguments.substr(first_space_idx	+ 1);

		packet.owner->privmsg( chan , msg );

	}, "says something on some chan. usage: @speak #chan msg...", REQUIRES_ADMIN_PERMS);

	b.on_privmsg("@slap ", [](const IRC::Packet& packet){
		packet.reply("\001ACTION slapped the hell outta " + packet.content.substr(6) + "\001");
	}, "slaps argument");

	b.on_privmsg("@google ", [](const IRC::Packet& packet){

		std::vector<std::string> res_vec;

		Googler::do_google_search(packet.content.substr(8), 2, res_vec);

		for (auto& res : res_vec) {
			packet.reply(res);
		}

	}, "returns first two results of a google search", REQUIRES_ADMIN_PERMS); // people have/will abuse this search... I can feel it.

	b.on_privmsg("@babble", [](const IRC::Packet& packet){
		packet.reply( babbler.sample() );
	}, "produces random technobabble", REQUIRES_ADMIN_PERMS);

	b.on_privmsg("@iplookup ", [](const IRC::Packet& packet){

		packet.reply( IPLookup::do_lookup( packet.content.substr( 10 ) ) );

	}, "looks up a specified IP.", REQUIRES_ADMIN_PERMS);

	b.on_privmsg("@kick ", [](const IRC::Packet& packet){
		packet.owner->kick(packet.channel, packet.content.substr(6));
	}, "kicks specified user.", REQUIRES_ADMIN_PERMS);

	b.on_privmsg("@join ", [](const IRC::Packet& packet){
		packet.owner->join_channel( packet.content.substr(6 , packet.content.find(" ", 6) - 6) );
	}, "joins specified channel.", REQUIRES_ADMIN_PERMS);

	b.on_privmsg("@part ", [](const IRC::Packet& packet){
		packet.owner->part_channel( packet.content.substr(6) );
	}, "parts a channel.", REQUIRES_ADMIN_PERMS);

	b.on_privmsg("@quit", [](const IRC::Packet& packet){
		packet.owner->disconnect( packet.content.length() > 6 ? packet.content.substr(6) : "Quitting..." );
	}, "quits server.", REQUIRES_ADMIN_PERMS);


	/* Add a server and connect to it by name */
	/* SSL is used by default. To disable it, pass `false` in place of the with_ssl arg. */
	for (auto& server : ENVIRONMENT["servers"]) {
		b.add_server( server["name"].get<std::string>(), server["address"].get<std::string>() , server["port"].get<int>(), server["with_ssl"].get<bool>());
		b.connect_server( server["name"].get<std::string>() );
	}

	/* Listen on the server(s) for input! */
	b.listen();


	return 0;
}
