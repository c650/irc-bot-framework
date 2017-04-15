/*
	reminders.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include <chrono>

class Reminder {
	std::string who; /* the user to remind */
	std::string what; /* what to remind */
	std::chrono::duration<unsigned long long> when; /* how long (in seconds) after `start` to trigger. */

	std::chrono::time_point<std::chrono::system_clock> start; /* when Reminder was set. */

	const IRC::Server* s;

  public:
	Reminder(const std::string& _who, const std::string& _what, const unsigned long long& _when, const IRC::Server* _s)
		: who(_who), what(_what), when(_when), start(std::chrono::system_clock::now()), s(_s) {}

	bool its_time(const IRC::Server *s) {
		return std::chrono::system_clock::now() - start >= when && s == this->s;
	}

	std::string format(void) {
		return who + ": Hey, you asked me to remind you about this: \'" + what + "\'";
	}

	std::string get_who(void) {
		return who;
	}
};

class ReminderCommand : protected IRC::CommandInterface {

	std::vector<Reminder*> reminders;

  public:

	ReminderCommand() : IRC::CommandInterface("@remind", "Reminds you of something in X minutes. Ex. @remind 30 take out trash.") {}

	~ReminderCommand() {
		for (Reminder* r : reminders)
			if (r) delete r;
	}

	bool triggered(const IRC::Packet& p) {
		reminders.resize(std::distance(reminders.begin(),std::remove_if(reminders.begin(), reminders.end(), [p](Reminder* r){
			if (!(r->its_time(p.owner)))
				return false;

			p.owner->privmsg(r->get_who(), r->format());

			delete r;
			return true;
		})));

		return IRC::CommandInterface::triggered(p);
	}

	void run(const IRC::Packet& p) {
		/* this will ADD a new Reminder */
		try {
			std::string command = p.content.substr(p.content.find(" ")+1); // skip over @remind

			size_t first_space = command.find(" ");
			if (first_space == std::string::npos) {
				p.reply("Error: you've not requested enough info. @help remind for more.");
				return;
			}

			reminders.push_back(new Reminder(p.sender, command.substr(first_space+1), std::stoll(command.substr(0, first_space)) * 60, p.owner));
			p.reply("Ok. I'll keep that in mind.");
		} catch (...) {
			p.reply("ERROR: Invalid input. See @help remind");
		}
	}
};

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new ReminderCommand);
}
