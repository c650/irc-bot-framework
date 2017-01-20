#ifndef __DYNAMIC_LOADING_H
#define __DYNAMIC_LOADING_H
/*

	dynamic-loading.hpp

	The goal here is to write some code that will allow
	an end user to load his own plugins without needed
	to recompile the bot or even have the bot's source code at all.

	https://www.linuxjournal.com/article/3687
*/

<<<<<<< HEAD
#include "./types.hpp"

#include <string>
#include <vector>
=======
#include "./command-interface.hpp"
#include "./bot.hpp"
>>>>>>> 6dad7aef7121520652f124a30bada37239c02c4d

namespace DynamicPluginLoading {

	class DynamicPlugin {

		void *raw_handle; // handle for use by dl functions.
		std::string name; // filepath to plugin.

		std::vector<IRC::CommandInterface*> instances;

	public:

		DynamicPlugin ( std::string n = "Unnamed");

		~DynamicPlugin();

		/*
			Returns an instance of the command interface
			defined in the plugin.

			Uses dlsym.
		*/
		IRC::CommandInterface* get_instance(IRC::Bot * b);

		std::string get_name(void) {
			return name;
		}

		bool provides_instance_of(IRC::CommandInterface* cptr);

	private:

		/*
			Loads the plugin.

			Uses dlopen().
		*/
		void load_plugin(void);

		/*
			Closes the plugin

			Uses dlclose().
		*/
		void close_plugin(void);

	};

};

#endif
