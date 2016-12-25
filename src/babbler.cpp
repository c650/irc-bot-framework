#include "./include/babbler.cpp"

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <random>

#include <unistd.h>

Babbler::Babbler(const std::string& path_to_babble_file) {

	random_number_gen.seed(getpid());

	try {
		std::fstream fs(path_to_babble_file, std::fstream::in);
		std::string tmp;
		while (std::getline(fs, tmp))
			babbles.push_back(tmp);
		fs.close();
	} catch (...) {
		std::cerr << "Error initializing Babbler with babbles.\n";
	}
}

std::string Babbler::sample(void) {
	return babbles.empty() ? std::string("No babbles available.") : babbles.at( random_number_gen() % babbles.size() );
}
