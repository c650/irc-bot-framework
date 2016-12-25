#ifndef _GOOGLER_H
#define _GOOGLER_H

#include <string>
#include "./json.hpp"

#ifndef DEFAULT_CONFIG_PATH
	#define DEFAULT_CONFIG_PATH "./config.json"
#endif

namespace Googler {

	/*
		Formats num_res_to_show results into

		title -> link
	*/
	void do_google_search(const std::string& query,
	                      const size_t& num_res_to_show,
						  std::vector<std::string>& res_vec,
						  const std::string& config_path = DEFAULT_CONFIG_PATH);
};

#endif
