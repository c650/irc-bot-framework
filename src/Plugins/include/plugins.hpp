#include "../../IRCBot/include/command-interface.hpp"

#include "./iplookup.hpp"
#include "./googler.hpp"
#include "./random-line-stream.hpp" /* for use in BabblerCommand */
#include "./stocks.hpp"
#include "./quotes.hpp"

namespace Plugins {

	class GoogleCommand : protected IRC::CommandInterface {

		unsigned long long times_executed;

	  public:
		GoogleCommand()                                                                          /* pointer to owning bot not needed. */
			: CommandInterface("@google ", "Performs google search and returns shortened links.", nullptr, true), times_executed(0) {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {
			std::vector<std::string> res_vec;

			Googler::do_google_search(p.content.substr(this->trigger().length()), 2, res_vec);

			for (auto& res : res_vec) {
				p.reply(res);
			}
			this->times_executed++;
		}

		std::string get_stats(void) {
			return "GoogleCommand -> Times Executed: " + std::to_string(this->times_executed);
		}

	};

	class IPLookupCommand : protected IRC::CommandInterface {

	  public:

		IPLookupCommand() : IRC::CommandInterface("@iplookup ", "looks up IP address.", nullptr, true) {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {
			p.reply(IPLookup::do_lookup(p.content.substr(this->trigger().length())));
		}
	};

	class SayHiCommand : protected IRC::CommandInterface {

	  public:

		SayHiCommand() : IRC::CommandInterface("@sayhi", "says hi.") {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0, this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {
			p.reply("Hello!");
		}
	};

	class SlapCommand : protected IRC::CommandInterface {

	  public:

		SlapCommand() : IRC::CommandInterface("@slap ", "slaps arguments.") {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {
			p.reply("\001ACTION slapped the hell outta " + p.content.substr(this->trigger().length()) + "\001");
		}

	};

	class SpeakCommand : protected IRC::CommandInterface {

	  public:

		SpeakCommand() : IRC::CommandInterface("@speak ", "says a message to channel: @speak <chan> <msg...>", nullptr, true) {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

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

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {
			p.reply(rls.sample());
		}

	};

	class StocksCommand : protected IRC::CommandInterface {

		unsigned long long queries_done;

	  public:

		StocksCommand() : CommandInterface("@stock ", "checks a stock ticker price."), queries_done(0) {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {
			p.reply(Stocks::get_stock_summary( p.content.substr(this->trigger().length()) ));
		}

		std::string get_stats(void) {
			return "Stock Queries Done: " + std::to_string(queries_done);
		}

	};

	class QuoteCommand : protected IRC::CommandInterface {

	  public:
		QuoteCommand() : CommandInterface("@quote", "delivers the quote of the day.") {}

		bool triggered(const IRC::Packet& p) {
			return p.type == IRC::Packet::PacketType::PRIVMSG && p.content.substr(0,this->trigger().length()) == this->trigger();
		}

		void run(const IRC::Packet& p) {
			p.reply(Quotes::get_random_quote());
		}

	};

	class UtilityCommands : protected IRC::CommandInterface {

	  public:

		UtilityCommands(IRC::Bot* b = nullptr) : IRC::CommandInterface("@kick,@join,@part,@quit,@nick", "does utility actions", b, true) {
			if (b == nullptr) {
				throw std::logic_error("In UtilityCommands, Bot *b cannot be nullptr!");
			}
		}

		bool triggered(const IRC::Packet& p) {
			std::string cmd = p.content.substr(0,5);
			return p.type == IRC::Packet::PacketType::PRIVMSG
				&& ( cmd == "@kick" || cmd == "@join" || cmd == "@part" || cmd == "@quit" || cmd == "@nick");
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
				}
			}

			if (cmd == "@quit") { // quit
				p.owner->disconnect( p.content.length() > 6 ? p.content.substr(6) : "Quitting..." );
			}
		}
	};

};
