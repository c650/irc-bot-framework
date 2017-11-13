#include "../IRCBot/include/user.hpp"

#include <iostream>

int main(void) {

	std::cout << "[***] TESTING USER CLASS\n";

	IRC::User* u = new IRC::User("chuck!chuck@0.0.0.0");


	delete u;

	return 0;
}
