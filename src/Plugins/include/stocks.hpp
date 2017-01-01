#ifndef _STOCKS_H
#define _STOCKS_H

#include <string>

namespace Stocks {

	/*
		Does a stock query on q.
		@param q the stock to search for, as a string.

		@return a formatted summary.
	*/
	std::string get_stock_summary(const std::string& q);

}

#endif
