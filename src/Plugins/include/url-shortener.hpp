#ifndef _URL_SHORTENER_H
#define _URL_SHORTENER_H

#include <string>
#include "./json.hpp"

namespace ShortenURL {

	/*
		Shortens a URL with the tinyurl api. Note the configuration
		necessary in (sample-)config.json

		@param long_url the long version of the url.
		@return the shortened url.
	*/
	std::string get_shortened_url(const std::string& long_url);

};

#endif
