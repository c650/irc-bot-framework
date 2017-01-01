#ifndef _QUOTES_H
#define _QUOTES_H

#include <string>

namespace Quotes {

	/*
		Returns a random quote from
		http://quotesondesign.com/wp-json/posts?filter[orderby]=rand&filter[posts_per_page]=1
	*/
	std::string get_random_quote(void);

};

#endif
