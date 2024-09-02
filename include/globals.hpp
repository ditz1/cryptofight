#pragma once
#include <emscripten/websocket.h>
#include <cstdio>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>


typedef struct Connection {
    EMSCRIPTEN_WEBSOCKET_T ws;
    EmscriptenWebSocketMessageEvent* last_received;
    bool connected;
} Connection;