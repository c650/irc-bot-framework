#include "./include/user.hpp"

#include <string>
#include <stdexcept>
#include <regex>

#include <iostream>

namespace IRC {

	User::User(const std::string& mask) {

		const static std::regex USER_REGEX{"([-\\w\\d]+)!([-\\w\\d]+)@([\\w\\d\\.]+)"};

		const static std::string error_msg = "Could not parse mask string into a user.";

		try {

			std::smatch match;
			if (!std::regex_search(mask, match, USER_REGEX)) {
				throw std::runtime_error(error_msg);
			}

			nick     = match[1];
			realname = match[2];
			hostname = match[3];

		} catch (...) {
			throw std::runtime_error(error_msg);
		}

	}

	User::User(const std::string& n, const std::string& hn) : User(n, n, hn) {}
	User::User(const std::string& n, const std::string& rn, const std::string& hn) : nick(n), realname(rn), hostname(hn) {}

	bool User::operator==(const User& other) const {
		return this == &other || this->hostname == other.hostname;
	}

	std::string User::get_nick(void) const {
		return nick;
	}

	void User::set_nick(const std::string new_nick) {
		nick = new_nick;
	}

	std::string User::get_realname(void) const {
		return realname;
	}

	void User::set_realname(const std::string new_realname) {
		realname = new_realname;
	}

	std::string User::get_hostname(void) const {
		return hostname;
	}

	void User::set_hostname(const std::string new_hostname) {
		hostname = new_hostname;
	}

	std::string User::get_mask(void) const {
		return nick + "!" + realname + "@" + hostname;
	}
}
