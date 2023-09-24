#include "websocket_utility.h"
#include <openssl/sha.h>
#include "crypto.h"
#include "base64.h"
#include <limits>
#include <bits/stdc++.h>
#include <bitset>

const std::string MAGIC_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

const std::string get_sec_websocket_accept_value(const std::string &sec_websocket_key)
{
    auto sha1_hash = sha1(sec_websocket_key + MAGIC_GUID);
    auto base64str = base64_encode(sha1_hash.data(), sha1_hash.size());

    return base64str;
}

void parse_websocket_frame(const char *frame, size_t frame_len)
{
    auto first_byte = frame[0];
    bool FIN = first_byte & (1 << (CHAR_BIT)) > 0;
    int opcode = first_byte & 15;

    auto is_masked = (frame[1] & 128) > 0;
    auto payload_len = frame[1] & 127;
    // Assume mask is on and small payload_len
    // let's unmask
    const char *mask_start = &frame[2];
    const char *payload_start = mask_start + 4;
    for (size_t i = 0; i < payload_len; ++i)
    {
        putchar(mask_start[i % 4] ^ payload_start[i]);
    }

    putchar('\n');
}