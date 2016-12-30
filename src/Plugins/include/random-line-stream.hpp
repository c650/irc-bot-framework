#ifndef _RAND_LINE_STREAM_H
#define _RAND_LINE_STREAM_H

#include <vector>
#include <string>
#include <random>

class RandomLineStream {

	std::vector<std::string> lines;
	std::minstd_rand random_number_gen;

public:

	RandomLineStream();
	RandomLineStream(const std::string& path_to_file);

	void load_file(const std::string& path_to_file);

	std::string sample(void);

};

#endif
