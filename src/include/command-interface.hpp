#ifndef _COMMAND_INTERFACE_H
#define _COMMAND_INTERFACE_H

#include "./packet.hpp"

#include <functional>
#include <string>

class CommandInterface {

	std::function<void(const Packet&)> func;

	std::string trigger_string, description;

	bool req_admin;

public:

	CommandInterface(const std::string& ts, const std::string& d = "No Description Set.", bool ra = false)
		: trigger_string(ts), description(d), func([](const Packet& p){return;}), requires_admin(ra) {}

	/*
		Inheritors will complete this function to
		determine whether or not a certain packet
		has the trigger for their command.

		@param p the packet to inspect.

		@return whether the packet is triggering.
	*/
	virtual bool triggered(const Packet& p) const = 0;

	/*
		Function can be overridden, but it defaults to calling func().

		@param p the packet to be passed to func.
	*/
	virtual void run(const Packet& p) const {
		func(p);
	}

	bool requires_admin(void) const {
		return req_admin;
	}

	std::string trigger(void) const {
		return trigger_string;
	}

	std::string desc(void) const {
		return description;
	}

}


#endif
