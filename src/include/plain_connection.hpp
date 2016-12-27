#ifndef _PLAIN_CONNECTION_H
#define _PLAIN_CONNECTION_H

#include <string>

namespace SSLWrapper {

	class PlainConnection {

	protected:
		int connection_socket_fd,
		    port;

		std::string address;

	public:

		PlainConnection(const std::string& _address, const int& port);
		~PlainConnection();

		PlainConnection(PlainConnection& other) = delete;
		PlainConnection& operator=(PlainConnection& other) = delete;

		bool do_connect(void);
		bool disconnect(void);

		bool is_connected(void) {
			return socket_fd > 0;
		};

		std::string receive(void);
		int send(const std::string& msg);

		bool is_secure(void) {
			return false;
		}

	};

};

#endif
