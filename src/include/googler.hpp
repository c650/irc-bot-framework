#ifndef _GOOGLER_H
#define _GOOGLER_H

#include <iostream>
#include <fstream>
#include <string>

#include<curl/curl.h>

#include "./json.hpp"
#include "./url-shortener.hpp"

#ifndef DEFAULT_CONFIG_PATH
	#define DEFAULT_CONFIG_PATH "./config.json"
#endif

namespace Googler {

	void get_json_from_web(const std::string& url , nlohmann::json& json_obj);
	size_t write_to_string(void *ptr, size_t size, size_t nmemb, std::string& stream);

	/*
		Formats num_res_to_show results into

		title
			snippet
			link
	*/
	void do_google_search(const std::string& formatted_query,
	                      const size_t& num_res_to_show,
						  std::vector<std::string>& res_vec,
						  const std::string& config_path = DEFAULT_CONFIG_PATH);
};

#endif
