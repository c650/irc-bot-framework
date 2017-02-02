#ifndef __USER_H
#define __USER_H

namespace IRC {

	class User {

		std::string nick,
		            realname,
					hostname;

	public:

		User(std::string n, hn);
		User(std::string n, rn, hn);

		bool operator==(const User& other);

		std::string get_nick();
		void set_nick(const std::string new_nick);

		std::string get_realname();
		void set_realname(const std::string new_realname);

		std::string get_hostname();
		void set_hostname(const std::string new_hostname);
	};
};

#endif
