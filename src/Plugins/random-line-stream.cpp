#include "./include/random-line-stream.hpp"

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <random>

#include <unistd.h>

RandomLineStream::RandomLineStream() {
	random_number_gen.seed(getpid());
}

RandomLineStream::RandomLineStream(const std::string& path_to_file) : RandomLineStream() {
	this->load_file(path_to_file);
}

std::string RandomLineStream::sample(void) {
	return lines.empty() ? std::string("No content available.") : lines.at( random_number_gen() % lines.size() );
}

void RandomLineStream::load_file(const std::string& path_to_file) {
	try {
		std::fstream fs(path_to_file, std::fstream::in);
		std::string tmp;
		while (std::getline(fs, tmp))
			lines.push_back(tmp);
		fs.close();
	} catch (...) {
		std::cerr << "Error initializing RandomLineStream with content.\n";
	}
}
