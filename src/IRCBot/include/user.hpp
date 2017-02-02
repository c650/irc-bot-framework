#ifndef __USER_H
#define __USER_H

#include <string>

namespace IRC {

	class User {

		std::string nick,
		            realname,
					hostname;

	public:

		/*
			Parses the passed mask into a nick, realname, and hostname.

			A valid mask: "nick!realname@hostname"
		*/
		User(std::string mask);

		User(const std::string& n, const std::string& hn);
		User(const std::string& n, const std::string& rn, const std::string& hn);

		bool operator==(const User& other) const;

		std::string get_nick(void) const;
		void set_nick(const std::string new_nick);

		std::string get_realname(void) const;
		void set_realname(const std::string new_realname);

		std::string get_hostname(void) const;
		void set_hostname(const std::string new_hostname);

		std::string get_mask(void) const;
	};
};

#endif
