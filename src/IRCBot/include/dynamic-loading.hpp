#ifndef __DYNAMIC_LOADING_H
#define __DYNAMIC_LOADING_H
/*

	dynamic-loading.hpp

	The goal here is to write some code that will allow
	an end user to load his own plugins without needed
	to recompile the bot or even have the bot's source code at all.

	https://www.linuxjournal.com/article/3687
*/

#include "./command-interface.hpp"

namespace DynamicPluginLoading {

	class DynamicPlugin {

		void *raw_handle; // handle for use by dl functions.
		std::string name; // filepath to plugin.

	public:

		DynamicPlugin ( std::string n = "Unnamed");

		~DynamicPlugin();

		/*
			Returns an instance of the command interface
			defined in the plugin.

			Uses dlsym.
		*/
		IRC::CommandInterface* get_instance(void);

		std::string get_name(void) {
			return name;
		}

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
