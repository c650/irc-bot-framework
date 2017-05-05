/*
	roll.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include <random>
#include <chrono>
#include <cctype>

class RollCommand : protected IRC::CommandInterface {

	std::minstd_rand gen;

	struct dice {
		int quantity, base;

		dice(std::string str) {
			for (auto& c : str)
				c = std::tolower(c);

			quantity = std::stoi(str);

			size_t find_d = str.find("d");
			if (find_d == std::string::npos) {
				throw std::runtime_error("Invalid str argument to dice construction: " + str);
			}

			str = str.substr(find_d + 1);
			base = std::stoi(str);

			/* limit how many dice to roll... */
			quantity = std::min(5, quantity);
		}

		std::string roll(std::minstd_rand& r) {
			if (base <= 0) return "0";
			std::string ret = "";
			for (int i = 0; i < quantity; ++i)
				ret += std::to_string(r() % base + 1) + " ";
			return ret;
		}
	};

  public:

	RollCommand() : IRC::CommandInterface("@roll", "Roll some dice. Ex. @dice 1d6 2d8") {
		gen.seed(std::chrono::system_clock::now().time_since_epoch().count());
	}

	void run(const IRC::Packet& p) {

		std::vector<std::string> args;
		p.get_args(args);

		std::string result = args.size() > 1  ? "Here: " : "You didn't specify what to roll.";

		try {
			for (size_t i = 1; i < args.size(); ++i) {
				dice tmp(args[i]);
				result += tmp.roll(gen);
			}
		} catch (std::exception& e) {
			std::cerr << "RollCommand::run() -> Something went wrong: " << e.what() << '\n';
			result = "Invalid arguments.";
		}

		p.reply(result);
	}
};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new RollCommand);
}
