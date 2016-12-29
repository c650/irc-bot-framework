#ifndef _BABBLER_H
#define _BABBLER_H

#include <vector>
#include <string>
#include <random>

class Babbler {

	std::vector<std::string> babbles;
	std::minstd_rand random_number_gen;

public:

	Babbler();
	Babbler(const std::string& path_to_babble_file);

	void load_file(const std::string& path_to_babble_file);

	std::string sample(void);

};

#endif
