#include "./include/user.hpp"

namespace IRC {

	User::User(std::string n, hn) : User(n, n, hn) {}
	User::User(std::string n, rn, hn) : nick(n), realname(rn), hostname(hn) {}

	bool User::operator==(const User& other) {
		return this == &other || this->nick == other.nick && this->realname == other.realname && this->hostname == other.hostname;
	}

	std::string User::get_nick() {
		return nick;
	}

	void User::set_nick(const std::string new_nick) {
		nick = new_nick;
	}

	std::string User::get_realname() {
		return realname;
	}

	void User::set_realname(const std::string new_realname) {
		realname = new_realname;
	}

	std::string User::get_hostname() {
		return hostname;
	}

	void User::set_hostname(const std::string new_hostname) {
		hostname = new_hostname;
	}
}
