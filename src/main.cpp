#include "./bot.hpp"
#include "./packet.hpp"

#include <unistd.h>

int main(void) {

	std::cout << getpid() << '\n';

	IRC::Bot b("test", "test", "test1");
	b.add_server("test","irc.0x00sec.org" , 6667);
	b.connect_server("test");

	b.on_privmsg("@sayhi", [](const IRC::Packet& packet){packet.reply("Hello!");});
	
	b.on_privmsg("@slap ", [](const IRC::Packet& packet){
		packet.reply("\001ACTION slapped the hell outta " + packet.content.substr(6) + "\001");
	});

	b.listen();


	return 0;
}
