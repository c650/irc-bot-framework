#include "./include/json.hpp" // from https://github.com/nlohmann/json
#include "./include/googler.hpp"

#include "./include/bot.hpp"
#include "./include/packet.hpp"

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <curl/curl.h>
#include <cstring>
#include <random>

#define REQUIRES_ADMIN_PERMS true

std::vector<std::string> babbles;
std::minstd_rand random_number_gen;

void format_query(const std::string& query_str, std::string& result);
static void readlines(const std::string& filename, std::vector<std::string>& result);

std::string get_a_babble() {
	return babbles.empty() ? std::string("No Babbles atm.") : babbles.at(random_number_gen() % babbles.size());
}

int main(void) {

	random_number_gen.seed(getpid());
	readlines("./customize/techno_babble.txt", babbles);

	/* Simple startup. Initialize a bot with a nick, password, and admin. */
	IRC::Bot b("pinetree", "hi", "oaktree");

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
		std::string query = "";
		format_query( packet.content.substr(8) , query );

		std::vector<std::string> res_vec;
		Googler::do_google_search(query, 2, res_vec);
		for (auto& res : res_vec) {
			packet.reply(res);
		}

	}, "returns first two results of a google search", REQUIRES_ADMIN_PERMS); // people have/will abuse this search... I can feel it.

	b.on_privmsg("@babble", [](const IRC::Packet& packet){
		packet.reply(get_a_babble());
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
	b.add_server("test","irc.0x00sec.org" , 6667);
	b.connect_server("test");

	/* Listen on the server(s) for input! */
	b.listen();


	return 0;
}

void format_query(const std::string& query_str, std::string& result) {

	CURL *curl = curl_easy_init();
	if (curl == nullptr) {
		return;
	}

	char *escaped;
	char *query_str_char_ptr = strdup(query_str.data());
	char *token = strtok(query_str_char_ptr, ",;?!-_ \n\r");

	while (token != nullptr) {

		escaped = curl_easy_escape(curl, token, strlen(token));

		std::cout << escaped << '\n';

		result.append(std::string(escaped) + '+');

		token = strtok(nullptr, ",.:;?!-_ ");

		curl_free(escaped);
	}

	if (!result.empty())
		result.pop_back(); // remove last '+';

	free(query_str_char_ptr);
	curl_easy_cleanup(curl);
}

static void readlines(const std::string& filename, std::vector<std::string>& result) {
	std::fstream fs(filename, std::fstream::in);

	std::string tmp;
	while(std::getline(fs, tmp))
		result.push_back(tmp);

	fs.close();
}
