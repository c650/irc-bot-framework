#ifndef _BASE_CONNECTION_H
#define _BASE_CONNECTION_H

#include <string>

namespace SSLWrapper {

	class BaseConnection {

	public:

		BaseConnection() {}

		virtual bool do_connect(void) = 0;
		virtual void disconnect(void) = 0;

		virtual bool is_connected(void) = 0;

		virtual std::string receive(void) = 0;
		virtual int send(const std::string& msg) = 0;

		virtual bool is_secure(void) = 0;

		virtual int socket_fd(void) const = 0;
		virtual std::string get_address(void) const = 0;
		virtual int get_port(void) const = 0;

		BaseConnection(BaseConnection& other) = delete;
		BaseConnection& operator=(BaseConnection& other) = delete;

	};

};

#endif
