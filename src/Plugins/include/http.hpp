#ifndef _MY_HTTP_H
#define _MY_HTTP_H

#include <iostream>
#include <curl/curl.h>

#include "./json.hpp"

#include "./environment.hpp"

namespace MyHTTP {

	/*
		Uses Libcurl to perform a get request to some URL.

		@param url the url to GET with HTTP

		@return the response data.
	*/
	std::string get(const std::string& url);


	/*
		Performs an HTTP POST

		@param url the target for POST
		@param params the post parameters to send.

		@return the response data.
	*/
	std::string post(const std::string& url, const std::string& params);

	// un-developed overload.
	std::string post(const std::string& url, const nlohmann::json& params);

	/*
		Encodes a string with URI compliance.

		@param unformatted the string to encode.
		@return the encoded string
	*/
	std::string uri_encode(const std::string& unformatted);

	/* CURL Handle struct for RAII*/
	struct CURLHandle {

		CURL* curl_ptr;

		CURLHandle();
		CURLHandle(CURL* _curl_ptr);

		~CURLHandle();

		/*
			Does basic setup stuff.
		*/
		template <typename WriteFunc>
		CURLHandle& do_setup(const std::string& url, WriteFunc func, std::string& response_string)  {
			if (curl_ptr == nullptr)
				curl_ptr = curl_easy_init();

			if (curl_ptr) {
				curl_easy_setopt(curl_ptr, CURLOPT_URL, url.data());
				curl_easy_setopt(curl_ptr, CURLOPT_WRITEFUNCTION, func);
				curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, response_string);

				if (!ENVIRONMENT["user_agent"].is_null())
					curl_easy_setopt(curl_ptr, CURLOPT_USERAGENT, ENVIRONMENT["user_agent"].get<std::string>().data());
				return *this;
			}
			throw std::runtime_error("Failed to initialize the CURL*");
		}

		/*
			Internally calls curl_easy_perform.
			Throws std::logic_error if curl_ptr is nullptr.
		*/
		CURLcode perform(void);

	};
};

#endif
