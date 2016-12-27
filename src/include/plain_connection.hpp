#ifndef _PLAIN_CONNECTION_H
#define _PLAIN_CONNECTION_H

#include <string>
#include "./base_connection.hpp"

namespace SSLWrapper {

	class PlainConnection : public BaseConnection {

	protected:
		int connection_socket_fd,
		    port;

		std::string address;

	public:

		PlainConnection(const std::string& _address, const int& port);
		~PlainConnection();

		bool do_connect(void);
		void disconnect(void);

		bool is_connected(void) {
			return connection_socket_fd > 0;
		};

		std::string receive(void);
		int send(const std::string& msg);

		bool is_secure(void) {
			return false;
		}

		bool operator==(PlainConnection& other) const {
			return false;
		}

		int socket_fd(void) const {
			return connection_socket_fd;
		}

		std::string get_address(void) const {
			return address;
		}
		int get_port(void) const {
			return port;
		}


	};

};

#endif
