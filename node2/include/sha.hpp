#ifndef __sha__
#define __sha__
#include <iostream>
#include <sstream>
#include "openssl/sha.h"

std::string to_hex(unsigned char s);
std::string sha256(std::string line);

#endif