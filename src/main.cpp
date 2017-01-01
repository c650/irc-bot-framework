#include "./Plugins/include/environment.hpp"

#include "./Plugins/include/json.hpp" /* from https://github.com/nlohmann/json */
#include "./Plugins/include/plugins.hpp"

#include "./IRCBot/include/bot.hpp"
#include "./IRCBot/include/packet.hpp"
#include "./IRCBot/include/command-interface.hpp"

#include <iostream>
#include <fstream>

#define DEFAULT_CONFIG_PATH "./config.json"

nlohmann::json ENVIRONMENT; /* Global Environment Variable. */

int main(int argc, char **argv) {

	/* Load the environment */
	try {
		std::fstream fs(argc > 1 ? argv[1] : DEFAULT_CONFIG_PATH, std::fstream::in);
		fs >> ENVIRONMENT;
		fs.close();
	} catch(std::exception& e) {
		std::cerr << "Couldn't open " << DEFAULT_CONFIG_PATH << ": " << e.what() << '\n';
		return 1;
	}


	/* Simple startup. Initialize a bot with a nick, password, and admin. */
	IRC::Bot b(ENVIRONMENT["bot"]["nick"].get<std::string>(), /* Nickname */
	           ENVIRONMENT["bot"]["pass"].get<std::string>(), /* Password, can be "" */
			   ENVIRONMENT["bot"]["admins"].get<std::vector<std::string>>()); /* List of admins. */
	// The program SHOULD crash if the above 3 lines don't work.

	b.add_command( (IRC::CommandInterface*)( new Plugins::SayHiCommand         ));
	b.add_command( (IRC::CommandInterface*)( new Plugins::GoogleCommand        ));
	b.add_command( (IRC::CommandInterface*)( new Plugins::IPLookupCommand      ));
	b.add_command( (IRC::CommandInterface*)( new Plugins::StocksCommand        ));
	b.add_command( (IRC::CommandInterface*)( new Plugins::QuoteCommand         ));
	b.add_command( (IRC::CommandInterface*)( new Plugins::SlapCommand          ));
	b.add_command( (IRC::CommandInterface*)( new Plugins::SpeakCommand         ));
	b.add_command( (IRC::CommandInterface*)( new Plugins::UtilityCommands(&b)  ));

	try {
		b.add_command( (IRC::CommandInterface*)(new Plugins::BabblerCommand( ENVIRONMENT["babble"]["filepath"].get<std::string>() ) ) );
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
