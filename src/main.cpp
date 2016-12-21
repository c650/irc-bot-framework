#include "./bot.hpp"
#include "./packet.hpp"

#include <unistd.h>

int main(void) {

	std::cout << getpid() << '\n';

	/* Simple startup. Initialize a bot with a nick, password, and admin. */
	IRC::Bot b("pinetree", "hi", "test1");

	/* And here are some actions/commands you can do! */
	b.on_privmsg("@sayhi", [](const IRC::Packet& packet){packet.reply("Hello!");});

	b.on_privmsg("@slap ", [](const IRC::Packet& packet){
		packet.reply("\001ACTION slapped the hell outta " + packet.content.substr(6) + "\001");
	});

	b.on_privmsg("@part ", [](const IRC::Packet& packet){
		packet.owner->part_channel( packet.content.substr(6) );
	});

	b.on_privmsg("@quit", [](const IRC::Packet& packet){
		packet.owner->disconnect( packet.content.length() > 6 ? packet.content.substr(6) : "Quitting..." );
	});


	/* Add a server and connect to it by name */
	b.add_server("test","irc.0x00sec.org" , 6667);
	b.connect_server("test");

	/* Listen on the server(s) for input! */
	b.listen();


	return 0;
}
