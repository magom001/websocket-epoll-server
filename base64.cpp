
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include "base64.h"
#include <cstring>

int base64_encode(const unsigned char *buffer, size_t length, char **output)
{ // Encodes a binary safe base 64 string
    BIO *bio, *b64;
    BUF_MEM *buffer_ptr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Ignore newlines - write everything in one line
    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &buffer_ptr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    *output = (*buffer_ptr).data;

    return (0); // success
}

std::string base64_encode(const unsigned char *buffer, size_t length)
{
    char *output;

    base64_encode(buffer, length, &output);

    return std::string(output, strlen(output));
}