#ifndef _URL_SHORTENER_H
#define _URL_SHORTENER_H

#include <string>

#ifndef DEFAULT_CONFIG_PATH
	#define DEFAULT_CONFIG_PATH "./config.json"
#endif

namespace ShortenURL {

	std::string get_shortened_url(const std::string& long_url, const std::string& config_path = DEFAULT_CONFIG_PATH);

};

#endif
