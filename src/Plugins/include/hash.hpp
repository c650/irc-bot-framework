#ifndef _PLUGIN_HASH_H
#define _PLUGIN_HASH_H

#include <openssl/sha.h>
#include <string>

namespace Hash {

	std::string sha256(const std::string& str);

};

#endif
