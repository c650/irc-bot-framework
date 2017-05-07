/*
	news.cpp
*/

#include "../IRCBot/include/command-interface.hpp"
#include "../IRCBot/include/bot.hpp"
#include "../IRCBot/include/packet.hpp"

#include <chrono>
#include <stdexcept>
#include <random>
#include <unordered_map>

#include "./include/json.hpp"
#include "./include/http.hpp"
#include "./include/url-shortener.hpp"

#include "./include/environment.hpp"

class NewsCommand : protected IRC::CommandInterface {

	std::string api_key, category, newsapi_url;

	const size_t MAX_NEWS;
	const std::chrono::hours MAX_NO_UPDATE_TIME;

	std::vector<std::string> sources; /* tracks name of sources. API will handle the URLs. */

	/* tracks if user has already been sent some article. */
	std::unordered_map<std::string, std::vector<bool>> news_track;

	struct Article {
		std::string title, desc, link;

		Article(const std::string& t, const std::string& d, const std::string& l)
			: title(t), desc(d), link(l) {}

		std::string to_string(void) {
			std::string url = ShortenURL::get_shortened_url(link);
			return title + " -> " + (url.empty() ? link : url);
		}
	};

	std::vector<Article> articles;
	std::chrono::system_clock::time_point last_time; /* last time news was updated... */

	void get_sources(void);
	void update_nicks(const std::string&, const std::string&);
	void get_news(void);
	void get_news_from_src(const std::string& source);

  public:

	NewsCommand() : IRC::CommandInterface("@news", "feeds you a news article. Powered by NewsApi.org"), MAX_NEWS(20), MAX_NO_UPDATE_TIME(std::chrono::hours(24)) {
		try {
			newsapi_url	= ENVIRONMENT["newsapi"]["newsapi_url"];
			category    = ENVIRONMENT["newsapi"]["category"];
			api_key	    = ENVIRONMENT["newsapi"]["api_key"];
			get_sources();
			last_time = std::chrono::system_clock::now();
		} catch (std::exception& e) {
			std::cerr << "Something wrong in NewsCommand::NewsCommand(): " << e.what() << '\n';
			newsapi_url = category = api_key = "";
		}
	}

	bool triggered(const IRC::Packet& p) {

		/* update nicks */
		if (p.type == IRC::Packet::PacketType::NICK)
			update_nicks(p.sender, p.content);

		/* update news sources */
		auto elapsed = std::chrono::system_clock::now() - last_time;
		if (articles.empty() || elapsed >= MAX_NO_UPDATE_TIME)
			get_news();
		last_time = std::chrono::system_clock::now();

		return IRC::CommandInterface::triggered(p);
	}

	void run(const IRC::Packet& p) {

		/*
			find news article that caller hasn't seen yet
			and return that
		*/
		if (articles.empty())
			p.owner->privmsg(p.sender,"empty.");

		p.owner->privmsg(p.sender,"One sec...");

		if (news_track.find(p.sender) == news_track.end())
			news_track[p.sender] = std::vector<bool>(articles.size(), false);

		size_t i = 0;
		while(i < articles.size() && news_track[p.sender][i])
			++i;

		if (i >= articles.size()) {
			p.reply("No new news at the moment.");
			return;
		}

		news_track[p.sender][i] = true;
		p.owner->privmsg(p.sender, articles.at(i).to_string());
	}
};

void NewsCommand::get_sources(void) {

	try {
		std::string call = MyHTTP::get(newsapi_url + "/sources?language=en&category=" + category);
		auto json_obj = nlohmann::json::parse(call);

		/* check if API fail. */
		if (json_obj["status"] != "ok") {
		 	sources.clear();
			return;
		}

		for (auto& s : json_obj["sources"])
			sources.push_back(s["id"].get<std::string>());
	} catch (std::exception& e) {
		std::cerr << "Error in NewsCommand::get_sources(): " << e.what() << '\n';
		return;
	}
}

void NewsCommand::update_nicks(const std::string& old, const std::string& _new) {
	// for (auto& pair : news_track) {
	// 	if (pair.first == old) {
	// 		pair.first = _new;
	// 		break;
	// 	}
	// }
}

template<typename T>
static void stuffle(std::vector<T>& arr);

void NewsCommand::get_news(void) {
	if (api_key.empty()) return; /* no way to access news */

	articles.clear(); /* flush any old articles out... */
	news_track.clear(); /* all new news so refresh news_track (all articles marked unread) */

	for(auto& source : sources) {
		get_news_from_src(source);
		if (articles.size() >= MAX_NEWS)
			break;
	}

	stuffle(articles);
}

template<typename T>
static void stuffle(std::vector<T>& arr) {
	static std::minstd_rand gen(std::chrono::system_clock::now().time_since_epoch().count());
	for (size_t i = 0; i < arr.size(); ++i)
		std::swap(arr[i], arr[gen() % arr.size()]);
}

void NewsCommand::get_news_from_src(const std::string& source) {

	try {
		std::string call = MyHTTP::get(newsapi_url + "/articles?sortBy=latest&source=" + source + "&apiKey=" + api_key);
		auto json_obj = nlohmann::json::parse(call);

		if (json_obj["status"] != "ok")
			return;

		for (auto& article : json_obj["articles"])
			articles.push_back(Article(article["title"].get<std::string>(),
			                           article["description"].get<std::string>(),
									   article["url"].get<std::string>()));

	} catch (std::exception& e) {
		std::cerr << "Error in NewsCommand::get_news_from_src(...): " << e.what() << '\n';
	}
}

extern "C" IRC::CommandInterface* maker(IRC::Bot* b = nullptr) {
	return (IRC::CommandInterface*)(new NewsCommand);
}
