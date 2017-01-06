#include <sstream>
#include <iomanip>
#include <string>
#include <openssl/sha.h>
#include <vector>

#ifdef DEBUG
#include <iostream>
#endif

namespace Hash {

	std::string sha256(const std::string& str) {

		SHA256_CTX sha256_context;
		SHA256_Init(&sha256_context);

		SHA256_Update(&sha256_context, str.c_str(), str.length());

		std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
		SHA256_Final(hash.data(), &sha256_context);

		std::stringstream ss;
		for (unsigned char& e : hash) {
			ss << std::hex << std::setw(2) << std::setfill('0') << (int)e;
		}

		#ifdef DEBUG
		std::cout << ss.str() << '\n';
		#endif

		return ss.str();
	}
};
