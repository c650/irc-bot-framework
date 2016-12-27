#ifndef _IPLOOKUP_H
#define _IPLOOKUP_H

#include "./json.hpp"

namespace IPLookup {

	/*
		Looks up a host and returns a string concatenation of City, Region, Country.

		@param host the host to look up. An IP address works best.
		@return the string concatenation of the location.
	*/
	std::string do_lookup(const std::string& host);

};

#endif
