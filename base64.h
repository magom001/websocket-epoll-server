#pragma once

#include <string>

int base64_encode(const unsigned char *buffer, size_t length, char **output);
std::string base64_encode(const unsigned char *buffer, size_t length);