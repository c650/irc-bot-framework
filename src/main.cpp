#include "./bot.hpp"
#include "./packet.hpp"

#include <unistd.h>

#define REQUIRES_ADMIN_PERMS true

int main(void) {

	std::cout << getpid() << '\n';

	/* Simple startup. Initialize a bot with a nick, password, and admin. */
	IRC::Bot b("pinetree", "hi", "oaktree");

	/* And here are some actions/commands you can do! */
	b.on_privmsg("@sayhi", [](const IRC::Packet& packet){
		packet.reply("Hello!");
	}, "says hello");

	b.on_privmsg("@slap ", [](const IRC::Packet& packet){
		packet.reply("\001ACTION slapped the hell outta " + packet.content.substr(6) + "\001");
	}, "slaps argument");

	b.on_privmsg("@kick ", [](const IRC::Packet& packet){
		packet.owner->kick(packet.channel, packet.content.substr(6));
	}, "kicks specified user.", REQUIRES_ADMIN_PERMS);

	b.on_privmsg("@join ", [](const IRC::Packet& packet){
		packet.owner->join_channel( packet.content.substr(6 , packet.content.find(" ")) );
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
