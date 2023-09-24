#include "crypto.h"
#include <openssl/sha.h>

std::vector<unsigned char> sha1(const std::string &input)
{
    std::vector<unsigned char> buffer(SHA_DIGEST_LENGTH);
    // unsigned char buffer[SHA_DIGEST_LENGTH];
    auto result = SHA1(reinterpret_cast<const unsigned char *>(input.c_str()), input.size(), buffer.data());

    return buffer;
}
