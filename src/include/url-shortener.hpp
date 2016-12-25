#ifndef _URL_SHORTENER_H
#define _URL_SHORTENER_H

#include "./json.hpp"

// for write_to_string:
#include "./googler.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>

#ifndef DEFAULT_CONFIG_PATH
	#define DEFAULT_CONFIG_PATH "./config.json"
#endif

namespace ShortenURL {

	std::string uri_encode_url(const std::string& unformatted);
	std::string get_shortened_url(const std::string& long_url, const std::string& config_path = DEFAULT_CONFIG_PATH);

};

#endif
