#include <dlfcn.h> /* for dlopen, dlclose, dlsym */

#include "./include/dynamic-loading.hpp"
#include "./include/command-interface.hpp"

#include <stdexcept>
#include <utility>

namespace DynamicPluginLoading {

	DynamicPlugin::DynamicPlugin(std::string n) : name(n) {
		load_plugin();
	}

	DynamicPlugin::~DynamicPlugin() {
		close_plugin();
	}

	IRC::CommandInterface* DynamicPlugin::get_instance(IRC::Bot * bot) {

		if (raw_handle == nullptr) {
			throw std::runtime_error("There is no plugin to instantiate.");
		}

		void *maker = dlsym(raw_handle , "maker");

		typedef IRC::CommandInterface* (*fptr)(IRC::Bot*);

		fptr func = reinterpret_cast<fptr>(reinterpret_cast<void*>(maker));

		return func(bot);
	}

	void DynamicPlugin::load_plugin(void) {
		if ( (raw_handle = dlopen( name.data() , RTLD_LAZY )) == nullptr ) {
			throw std::runtime_error("Could not open " + name + ": " + dlerror());
		}
	}

	void DynamicPlugin::close_plugin(void) {
		if ( dlclose(raw_handle) != 0 ) {
			throw std::runtime_error("There was a problem closing: " + name);
		}
	}

};
