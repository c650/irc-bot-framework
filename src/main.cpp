#include "./Plugins/include/environment.hpp"

#include "./Plugins/include/json.hpp" /* from https://github.com/nlohmann/json */
#include "./Plugins/include/googler.hpp"
#include "./Plugins/include/random-line-stream.hpp"
#include "./Plugins/include/iplookup.hpp"
#include "./Plugins/include/sayhi.hpp"
#include "./Plugins/include/babbler.hpp"
#include "./Plugins/include/slap.hpp"
#include "./Plugins/include/speak.hpp"
#include "./Plugins/include/utility.hpp"

#include "./IRCBot/include/bot.hpp"
#include "./IRCBot/include/packet.hpp"
#include "./IRCBot/include/command-interface.hpp"

#include <iostream>
#include <fstream>

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

	b.add_command( (IRC::CommandInterface*)(new SayHi) );
	b.add_command( (IRC::CommandInterface*)(new Googler::GoogleCommand) );
	b.add_command( (IRC::CommandInterface*)(new IPLookup::IPLookupCommand) );
	b.add_command( (IRC::CommandInterface*)(new Slap) );
	b.add_command( (IRC::CommandInterface*)(new Speak) );
	b.add_command( (IRC::CommandInterface*)(new Utility) );

	try {
		b.add_command( (IRC::CommandInterface*)(new Babbler( ENVIRONMENT["babble"]["filepath"].get<std::string>() ) ) );
	} catch (...) {
		std::cerr << "Failed to add Babbler\n";
	}


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