#include "./include/user.hpp"

#include <string>
#include <stdexcept>

namespace IRC {

	User::User(std::string mask) {
		static std::string error_msg = "Could not parse mask string into a user.";

		try {
			size_t exclam = mask.find("!");
			if (exclam == std::string::npos) {
				throw std::runtime_error(error_msg);
			}

			nick = mask.substr(0, exclam);

			size_t at_char = mask.find("@", exclam+1);
			if (at_char == std::string::npos) {
				throw std::runtime_error(error_msg);
			}

			realname = mask.substr(exclam+1 , at_char - exclam - 1);

			hostname = mask.substr(at_char+1);
		} catch (...) {
			throw std::runtime_error(error_msg);
		}

	}

	User::User(const std::string& n, const std::string& hn) : User(n, n, hn) {}
	User::User(const std::string& n, const std::string& rn, const std::string& hn) : nick(n), realname(rn), hostname(hn) {}

	bool User::operator==(const User& other) const {
		return this == &other || this->nick == other.nick && this->realname == other.realname && this->hostname == other.hostname;
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
