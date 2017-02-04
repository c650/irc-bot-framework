/*
	weather.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include "./include/environment.hpp"
#include "./include/http.hpp"
#include "./include/json.hpp"

#include <stdexcept>

class WeatherCommand : protected IRC::CommandInterface {

  public:

	WeatherCommand() : IRC::CommandInterface("@weather ", "queries for weather") {}

	void run(const IRC::Packet& p) {
		std::string query = p.content.substr(this->trigger_string.length());

		if (query.empty()) return;

		query = MyHTTP::uri_encode(query);

		try {
			std::string search_response = "";
			MyHTTP::get( "http://autocomplete.wunderground.com/aq?query=" + query , search_response );

			nlohmann::json result = nlohmann::json::parse(search_response);
			result = result["RESULTS"].at(0);

			std::string lat_long = result["lat"].get<std::string>() + "," + result["lon"].get<std::string>();

			search_response = "";
			std::string url = "http://api.wunderground.com/api/" + ENVIRONMENT["WUNDERGROUND_KEY"].get<std::string>() + "/geolookup/q/" + lat_long + ".json";

			MyHTTP::get( url , search_response );
			result = nlohmann::json::parse(search_response);

			std::string locale = result["location"]["country"].get<std::string>() == "US" ?
									result["location"]["state"].get<std::string>() : result["location"]["country"].get<std::string>();

			std::string city = result["location"]["city"].get<std::string>();
			for (auto& c : city)
				 if (c == ' ')
				 	c = '_';

			search_response = "";
			MyHTTP::get( "http://api.wunderground.com/api/" + ENVIRONMENT["WUNDERGROUND_KEY"].get<std::string>() + "/conditions/q/" + locale + "/" + city + ".json",
							search_response );
			result = nlohmann::json::parse(search_response);
			result = result["current_observation"];

			std::string weather_result = "Weather for " + result["display_location"]["full"].get<std::string>()
								+ ": " + result["weather"].get<std::string>() + ", " + result["temperature_string"].get<std::string>()
								+ ", feels like: " + result["feelslike_string"].get<std::string>()
								+ ", Wind: " + std::to_string(result["wind_mph"].get<double>()).substr(0,4) + " mph with direction "
								+ result["wind_dir"].get<std::string>();

			p.reply( weather_result );

		} catch (std::exception& e) {
			std::cerr << "Error in WeatherCommand::run(): " << e.what() << '\n';
			p.reply("Failed to perform weather query.");
		}
	}
};

extern "C" {
	IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
		return (IRC::CommandInterface*)(new WeatherCommand);
	}
};
