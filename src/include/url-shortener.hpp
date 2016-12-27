#ifndef _URL_SHORTENER_H
#define _URL_SHORTENER_H

#include <string>
#include "./json.hpp"

namespace ShortenURL {

	std::string get_shortened_url(const std::string& long_url);

};

#endif
