#ifndef _GOOGLER_H
#define _GOOGLER_H

#include <iostream>
#include <fstream>
#include <string>

#include<curl/curl.h>

#include "./json.hpp"

#define GOOGLE_CONFIG_PATH ""

namespace Googler {

	void get_json_from_web(const std::string& url , nlohmann::json& json_obj);
	size_t write_to_string(void *ptr, size_t size, size_t nmemb, std::string& stream);

	/*
		Formats num_res_to_show results into

		title
			snippet
			link
	*/
	void do_google_search(const std::string& formatted_query, const size_t& num_res_to_show, std::vector<std::string>& res_vec, const std::string& api_key_path = "");

// ------------------------------------------------------------------------------

	void get_json_from_web(const std::string& url , nlohmann::json& json_obj) {

		CURL *curl = curl_easy_init();
		if (curl) {

			curl_easy_setopt(curl, CURLOPT_URL, url.data());

			std::string response;
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

			CURLcode res = curl_easy_perform(curl);
			if (res == CURLE_OK) {
				json_obj = nlohmann::json::parse(response);
			}
			curl_easy_cleanup(curl);
		}
	}

	size_t write_to_string(void *ptr, size_t size, size_t nmemb, std::string& stream) {
		size_t realsize = size * nmemb;
		std::string temp(static_cast<const char*>(ptr), realsize);
		stream.append(temp);
		return realsize;
	}

	void do_google_search(const std::string& formatted_query, const size_t& num_res_to_show, std::vector<std::string>& res_vec, const std::string& api_key_path) {

		// requires a JSON file with google API stuff
		nlohmann::json google_api_info;

		try {
			std::fstream fs( api_key_path.empty() ? GOOGLE_CONFIG_PATH : api_key_path, std::fstream::in);
			fs >> google_api_info;
		} catch (...) {
			std::cerr << "File IO failed. Couldn't retrieve API Key.\n";
			return;
		}

		nlohmann::json search_result;
		get_json_from_web(google_api_info["address"].get<std::string>()
							+ "key=" + google_api_info["key"].get<std::string>()
							+ "&cx=" + google_api_info["cx"].get<std::string>()
							+ "&q=" + formatted_query, search_result);

		size_t count = search_result["items"].size() > num_res_to_show ? num_res_to_show : search_result["items"].size();
		for (auto& item : search_result["items"]) {

			res_vec.push_back( item["title"].get<std::string>() + "->" /*+ item["snippet"].get<std::string>() + "->"*/ + item["link"].get<std::string>() );

			if (!(--count))
				break;
		}
	}
};

#endif
