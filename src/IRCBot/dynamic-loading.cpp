#include <dlfcn.h> /* for dlopen, dlclose, dlsym */

#include "./include/dynamic-loading.hpp"
#include "./include/command-interface.hpp"
#include "./include/bot.hpp"

#include <stdexcept>
#include <utility>

namespace DynamicPluginLoading {

	DynamicPlugin::DynamicPlugin(std::string n) : name(n) {
		load_plugin();
	}

	DynamicPlugin::~DynamicPlugin() {
		close_plugin();

		/* note that any cached instances are not deleted here
			because they were passed to bots who should handle the
			deletions. */
	}

	IRC::CommandInterface* DynamicPlugin::get_instance(IRC::Bot * bot) {

		if (raw_handle == nullptr) {
			throw std::runtime_error("There is no plugin to instantiate.");
		}

		void *maker = dlsym(raw_handle , "maker");

		if (maker == nullptr) {
			throw std::runtime_error("No maker() factory function is present!");
		}

		typedef IRC::CommandInterface* (*fptr)(IRC::Bot*);

		fptr func = reinterpret_cast<fptr>(reinterpret_cast<void*>(maker));

		this->instances.push_back(func(bot)); /* cache here to deallocate responsibly. */

		return this->instances.back();
	}

	void DynamicPlugin::load_plugin(void) {
		if ( (raw_handle = dlopen( name.data() , RTLD_NOW | RTLD_LOCAL )) == nullptr ) {
			throw std::runtime_error("Could not open " + name + ": " + dlerror());
		}
	}

	void DynamicPlugin::close_plugin(void) {
		if ( dlclose(raw_handle) != 0 ) {
			throw std::runtime_error("There was a problem closing: " + name);
		}
	}

	bool DynamicPlugin::provides_instance_of(const IRC::CommandInterface* cptr) const {
		for (auto c : instances) {
			if (c == cptr) {
				return true;
			}
		}
		return false;
	}

};
