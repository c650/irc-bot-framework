#include "../../IRCBot/include/command-interface.hpp"
#include "../../IRCBot/include/bot.hpp"

#include "./random-line-stream.hpp" /* for use in BabblerCommand */
#include "./iplookup.hpp"
#include "./googler.hpp"
#include "./stocks.hpp"
#include "./quotes.hpp"
#include "./http.hpp"
#include "./hash.hpp"
#include "./urban.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <random>
#include <cctype>

#include <unistd.h> /* getpid() */

namespace Plugins {

	class GoogleCommand : protected IRC::CommandInterface {

		unsigned long long times_executed;

	  public:
		GoogleCommand()                                                                          /* pointer to owning bot not needed. */
			: CommandInterface("@google ", "Performs google search and returns shortened links.", nullptr, true), times_executed(0) {}

		void run(const IRC::Packet& p) {
			std::vector<std::string> res_vec;

			Googler::do_google_search(p.content.substr(this->trigger().length()), 2, res_vec);

			for (auto& res : res_vec) {
				p.reply(res);
			}
			this->times_executed++;
		}

		std::string get_stats(void) const {
			return "GoogleCommand -> Times Executed: " + std::to_string(this->times_executed);
		}

	};

	class BabblerCommand : protected IRC::CommandInterface {

		RandomLineStream rls;

	  public:

		BabblerCommand(const std::string& babbles_filepath)
			: CommandInterface("@babble", "does a babble."), rls(babbles_filepath) {}

		void run(const IRC::Packet& p) {
			p.reply(rls.sample());
		}

	};

	class ChooseCommand : protected IRC::CommandInterface {

		std::minstd_rand random_gen;

	public:

		ChooseCommand() : CommandInterface("@choose ", "chooses a thing of comma-separated list.") {
			random_gen.seed(getpid());
		}

		bool triggered(const IRC::Packet& p) {
			if (p.content.length() < this->trigger_string.length())
				return false;

			std::string s = p.content.substr(0, this->trigger_string.length()-1);
			return p.type == IRC::Packet::PacketType::PRIVMSG && !std::isalpha(s[0]) && !std::isdigit(s[0]) && s.substr(1) == this->trigger_string.substr(1, s.length()-1);
		}

		void run(const IRC::Packet& p) {
			std::string choices_str = p.content.substr(this->trigger_string.length());

			std::vector<std::string> choices_vec;

			size_t last = 0, next = 0;
			while (last < choices_str.length() && (next = choices_str.find(",", last)) != std::string::npos) {
				choices_vec.push_back(choices_str.substr( last, next - last ) );

				last = next+1;
				while (last < choices_str.length() && choices_str[last] == ',')
					++last;
			}
			choices_vec.push_back(choices_str.substr(last));

			std::cout << "choices_vec.size() = " << choices_vec.size() << '\n';

			if ( !choices_vec.empty() )
				p.reply(p.sender + ": " + choices_vec.at( random_gen() % choices_vec.size() ) );
		}

	};

	class RecoveryCommand : public IRC::CommandInterface {

	  public:

		RecoveryCommand(IRC::Bot *b = nullptr) : CommandInterface("@recover ", "recover the bot with a password.", b) {}

		void run(const IRC::Packet& p) {

			std::string password = p.content.substr(this->trigger().length());

			if ( bot_ptr->reauthenticate(p.sender, Hash::sha256(password)) ) {
				p.owner->privmsg(p.sender, "Password accepted! You now have admin rights.");
			} else {
				p.owner->privmsg(p.sender, "Denied. Password invalid.");
			}
		}
	};

};
