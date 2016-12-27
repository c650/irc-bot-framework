#include "./include/json.hpp" /* from https://github.com/nlohmann/json */
#include "./include/googler.hpp"
#include "./include/babbler.hpp"

#include "./include/bot.hpp"
#include "./include/packet.hpp"

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <curl/curl.h>
#include <cstring>
#include <random>

#define REQUIRES_ADMIN_PERMS true

#ifndef DEFAULT_CONFIG_PATH
	#define DEFAULT_CONFIG_PATH "./config.json"
#endif

#ifndef WITH_SSL
	#define WITH_SSL true
#endif

Babbler babbler("./customize/techno_babble.txt");

int main(void) {

	/* Simple startup. Initialize a bot with a nick, password, and admin. */
	IRC::Bot b("pinetree", "", "oaktree");

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

		Googler::do_google_search(packet.content.substr(8), 2, res_vec, DEFAULT_CONFIG_PATH);

		for (auto& res : res_vec) {
			packet.reply(res);
		}

	}, "returns first two results of a google search", REQUIRES_ADMIN_PERMS); // people have/will abuse this search... I can feel it.

	b.on_privmsg("@babble", [](const IRC::Packet& packet){
		packet.reply( babbler.sample() );
	}, "produces random technobabble");

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
	/* SSL is used by default. To disable it, pass `false` in place of WITH_SSL */
	b.add_server("test","irc.0x00sec.org" , 6697, WITH_SSL);
	b.connect_server("test");

	/* Listen on the server(s) for input! */
	b.listen();


	return 0;
}
