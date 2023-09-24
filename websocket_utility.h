#pragma once

#include <string>

const std::string get_sec_websocket_accept_value(const std::string &);

struct WebsocketPacket
{
    int FIN;
    int RSV1;
    int RSV2;
    int RSV3;
    int Opcode;
    int Mask;
    int len;
};

void parse_websocket_frame(const char *frame, size_t frame_len);