#ifndef _MY_HTTP_H
#define _MY_HTTP_H

#include <iostream>
#include <curl/curl.h>

#include "./json.hpp"

namespace MyHTTP {

	/*
		Uses Libcurl to perform a get request to some URL.

		@param url the url to GET with HTTP
		@param response the string in which the response will be
			stored. It is up to the user to parse it to JSON or whatever.

		@return whether or not the response was a CURLE_OK.
	*/
	bool get(const std::string& url, std::string& response);


	/*
		Performs an HTTP POST

		@param url the target for POST
		@param params the post parameters to send.

		@return if POST was successful.
	*/
	bool post(const std::string& url, const nlohmann::json& params);

	/*
		Encodes a string with URI compliance.

		@param unformatted the string to encode.
		@return the encoded string
	*/
	std::string uri_encode(const std::string& unformatted);

};

#endif
