# An IRC Bot Framework

This framework is written entirely in C++. I wrote it because I felt like it.
Anyone is free to customize it, add to it, or even contribute in making a better, cleaner (please) framework.

---
# Usage

The code in [src/IRCBot](/src/IRCBot) is what is needed to use this framework.

To make a plugin/command/feature, you define a class that inherits from `CommandInterface`. This class has two important methods to be overridden:
1. `triggered` determines whether a certain packet's content should trigger the command.
2. `run` performs whatever action should happen when a packet triggers the command.

There are two ways to create plugins for a bot:
1. In a header file, included in [main.cpp](/src/main.cpp), and added directly
2. Or, defined in a source file, compiled separately, and loaded/unloaded dynamically.

### Method 1

	// src/Plugins/include/thing-plugin.hpp
    #ifndef _NAME_OF_THING_H
	#define _NAME_OF_THING_H

	#include "../../IRCBot/include/command-interface.hpp"
	#include "../../IRCBot/include/packet.hpp"

	class Thing : public CommandInterface {

	public:

		Thing() : CommandInterface("trigger as a string for help menu", "description", /* whether or not admin privs are needed. */) {}

		/* Note that constructors may specify parameters as needed. */

		bool triggered(const Packet& p) {
			/* what triggers the command */
		}

		void run(const Packet& p) {
			/* what to do when command is triggered */
		}

	}

	#endif

Once you define the class, include the header in [main.cpp](/src/main.cpp).

Use the following line to add the command to a bot:

	#include "./Plugins/include/thing-plugin.hpp"
	// ...
    some_bot.add_command( (IRC::CommandInterface*)(new Thing) );

---
### Method 2

	// src/Plugins/thing.cpp

	#include "../../IRCBot/include/command-interface.hpp"
	#include "../../IRCBot/include/packet.hpp"

	class Thing : public CommandInterface {

	public:

		Thing() : CommandInterface("trigger as a string for help menu", "description", /* whether or not admin privs are needed. */) {}

		/* Note that constructors may specify parameters as needed. */

		bool triggered(const Packet& p) {
			/* what triggers the command */
		}

		void run(const Packet& p) {
			/* what to do when command is triggered */
		}

	}

	extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new Thing);
	}

Compile it separately:

	$ g++ -Os --std=c++17 -fPIC -rdynamic -shared -o ./bin/thing.so ./src/Plugins/thing.cpp

Load it into the bot:

	<you> @load ./bin/thing.so
	<bot> Success

You can also have the plugin loaded at bot startup by including it in the `"dynamic_commands"` array in your configuration JSON:

	// ...
	"bot" : {
		"nick":"bot",
		"pass":"",
		"admins": [
			"YOU"
		],
		"ignore" : [],
		"recovery_sha256" : "SHA256 HASH OF PASSWORD",
		"dynamic_commands" : [
			"thing" // key
		],
		"dynamic_lib_path_prefix" : "./bin/"
	},
	// ...

---

To run the bot, information must be specified like it is in the [sample config](/sample-config.json).

---
# Compiling

First, run `./configure.sh`. If you aren't using a Debian-based linux, the dependencies are **libcurl** and **openSSL/libssl** and you must install them manually. If you intend to compile from source, the `*-dev` packages, as well as a C++ compiler, are also needed.

A 64-bit linux binary is on the releases page. For any other architecture or OS, please do the following:

	mkdir bin
	make
	./bin/bot.out [optional argument with path to configuration file]

**NOTE**: You may want to run the bot from inside a Docker container.

---

I have added a few commands as an example, but they needn't stay if you don't want them to.

---
# Acknowledgements

Thanks to @nlohmann for his [C++ JSON Library](https://github.com/nlohmann/json).

---
# License

This project is licensed under the [MIT License](/LICENSE).
