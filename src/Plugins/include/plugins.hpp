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

	class LMGTFYCommand : protected IRC::CommandInterface {

	  public:

		LMGTFYCommand()
			: CommandInterface("@lmgtfy ", "mean way to tell ppl to google things.") {}

		void run(const IRC::Packet& p) {
			p.reply("http://lmgtfy.com/?q=" + MyHTTP::uri_encode( p.content.substr(this->trigger().length()) ) );
		}

	};

	class UrbanCommand : protected IRC::CommandInterface {

	public:

		UrbanCommand()
			: CommandInterface("@urban ", "checks urban dictionary for a definition.") {}

		void run(const IRC::Packet& p) {
			std::string def = "";
			Urban::get_first_result(p.content.substr(this->trigger_string.length()) , def);
			if (!def.empty()) {
				p.reply(def);
			}
		}

	};

	class IPLookupCommand : protected IRC::CommandInterface {

	  public:

		IPLookupCommand() : IRC::CommandInterface("@iplookup ", "looks up IP address.", nullptr, true) {}

		void run(const IRC::Packet& p) {
			p.reply(IPLookup::do_lookup(p.content.substr(this->trigger().length())));
		}
	};

	class SayHiCommand : protected IRC::CommandInterface {

	  public:

		SayHiCommand() : IRC::CommandInterface("@sayhi", "says hi.") {}

		void run(const IRC::Packet& p) {
			p.reply("Hello!");
		}
	};

	class SlapCommand : protected IRC::CommandInterface {

	  public:

		SlapCommand() : IRC::CommandInterface("@slap ", "slaps arguments.") {}

		void run(const IRC::Packet& p) {
			p.reply("\001ACTION slapped the hell outta " + p.content.substr(this->trigger().length()) + "\001");
		}

	};

	class SpeakCommand : protected IRC::CommandInterface {

	  public:

		SpeakCommand() : IRC::CommandInterface("@speak ", "says a message to channel: @speak <chan> <msg...>", nullptr, true) {}

		void run(const IRC::Packet& p) {
			std::string arguments = p.content.substr(this->trigger().length()); // everything after "@speak "

			size_t first_space_idx = arguments.find(" ");
			if (first_space_idx == std::string::npos)
				return;
			std::string chan = arguments.substr(0, first_space_idx);
			std::string msg = arguments.substr(first_space_idx	+ 1);

			p.owner->privmsg( chan , msg );
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

	class StocksCommand : protected IRC::CommandInterface {

		unsigned long long queries_done;

	  public:

		StocksCommand() : CommandInterface("@stock ", "checks a stock ticker price."), queries_done(0) {}

		void run(const IRC::Packet& p) {
			p.reply(Stocks::get_stock_summary( p.content.substr(this->trigger().length()) ));
			this->queries_done++;
		}

		std::string get_stats(void) const {
			return "Stock Queries Done: " + std::to_string(queries_done);
		}

	};

	class QuoteCommand : protected IRC::CommandInterface {

	  public:
		QuoteCommand() : CommandInterface("@quote", "delivers the quote of the day.") {}

		void run(const IRC::Packet& p) {
			p.reply(Quotes::get_random_quote());
		}

	};

	class EliteCommand : protected IRC::CommandInterface {

		const std::string reglr = "abeiolgsqtz";
		const std::string elite = "48310195972";

	public:

		EliteCommand() : CommandInterface("@1337 ", "translates to and from 13375p34k.") {}

		void run(const IRC::Packet& p) {
			std::string sub = p.content.substr(this->trigger_string.length());

			std::string msg = "";

			try {
				msg = sub.substr(sub.find(" "));
			} catch (std::exception& e) {
				std::cerr << "EliteCommand::run() error: " << e.what() << '\n';
				p.reply("Error. Usage: @1337 [to1337 | from1337] [message...]");
			}

			sub = sub.substr(0, sub.find(" "));
			if (sub == "from1337") {
				for (auto& e : msg) {
					for (size_t i = 0; i < elite.length(); ++i) {
						if (std::tolower(e) == elite[i])
							e = reglr[i];
					}
				}
			} else {
				for (auto& e : msg) {
					for (size_t i = 0; i < reglr.length(); ++i) {
						if (std::tolower(e) == reglr[i])
							e = elite[i];
					}
				}
			}
			p.reply(msg);
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

	class UtilityCommands : protected IRC::CommandInterface {

	  public:

		UtilityCommands(IRC::Bot *b = nullptr) : IRC::CommandInterface("@kick, @join, @part, @quit, @nick, @adda", "does utility actions", b, true) {
			if (b == nullptr) {
				throw std::logic_error("In UtilityCommands, Bot *b cannot be nullptr!");
			}
		}

		bool triggered(const IRC::Packet& p) {
			std::string cmd = p.content.substr(0,5);
			return p.type == IRC::Packet::PacketType::PRIVMSG
				&& ( cmd == "@kick" || cmd == "@join" || cmd == "@part" || cmd == "@quit" || cmd == "@nick" || cmd == "@adda");
		}

		void run(const IRC::Packet& p) {
			std::string cmd = p.content.substr(0,5);

			if (p.content.length() > 5) {
				if ( cmd == "@kick" ) {
					p.owner->kick(p.channel, p.content.substr(6));
				} else if ( cmd == "@join" ) {
					p.owner->join_channel( p.content.substr(6 , p.content.find(" ", 6) - 6) );
				} else if ( cmd == "@part" ) {
					p.owner->part_channel( p.content.substr(6) );
				} else if ( cmd == "@nick" ) {
					p.owner->set_nick( p.content.substr(6) );
				} else if ( cmd == "@adda" ) {
					std::vector<std::string> args;
					p.get_args(args);
					if (args.size() < 3) {
						p.owner->privmsg(p.sender, "Error. Usage: @adda TYPE name");
						return;
					}

					try {
						std::transform(args[1].begin(), args[1].end(), args[1].begin(), ::tolower);
					} catch (...) {
						std::cerr << "failed in UtilityCommands @adda to transform to lowercase : " << args[1] << '\n';
						return;
					}
					IRC::Bot::RELATIONSHIP r = IRC::Bot::RELATIONSHIP::IGNORED;
					if (args[1] == "admin") {
						r = IRC::Bot::RELATIONSHIP::ADMIN;
					} else if (args[1] != "ignore") { // all else except "admin" and "ignore" (case insensitive)
						p.owner->privmsg(p.sender, "Error. Invalid argument: " + args[1]);
						return;
					}

					bot_ptr->add_a(r , args[2]);
					p.reply("Added " + args[2] + " to " + args[1] + " list.");
				}
			}

			if (cmd == "@quit") { // quit
				p.owner->disconnect( p.content.length() > 6 ? p.content.substr(6) : "Quitting..." );
			}
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
