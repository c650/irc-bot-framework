#ifndef _GOOGLER_H
#define _GOOGLER_H

#include <string>
#include "./json.hpp"

namespace Googler {

	/*
		Performs a google search and then formats the results.

		@param query the query to google for. It's URI encoded inside the function,
			meaning that a human-readable query should be passed.
		@param num_res_to_show the number of google results desired.
		@res_vec a string vector in which the results are stored. "Title -> ShortenedLink"
	*/
	void do_google_search(const std::string& query,
	                      const size_t& num_res_to_show,
						  std::vector<std::string>& res_vec);
};

#endif
