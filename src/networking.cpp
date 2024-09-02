#include "networking.hpp"
#include <iostream>

void OpenWebSocket(Connection* conn, const char* url) {    
    if (!emscripten_websocket_is_supported()) {
        std::cout << "WebSockets are not supported" << std::endl;
        return;
    }

    EmscriptenWebSocketCreateAttributes ws_attrs = {
        url, nullptr, EM_TRUE
    };

    conn->ws = emscripten_websocket_new(&ws_attrs);
    if (conn->ws <= 0) {
        std::cout << "Failed to create WebSocket" << std::endl;
    } else {
        conn->connected = false;  // Will be set to true in OnOpen callback
        emscripten_websocket_set_onopen_callback(conn->ws, conn, OnOpen);
        emscripten_websocket_set_onmessage_callback(conn->ws, conn, OnMessage);
    }
}

void CloseWebSocket(EMSCRIPTEN_WEBSOCKET_T ws) {
    emscripten_websocket_close(ws, 1000, "Closed");
    emscripten_websocket_delete(ws);
}

void ClientSendBytes(EMSCRIPTEN_WEBSOCKET_T ws, void* data, uint32_t length) {
    EM_BOOL error = emscripten_websocket_send_binary(ws, data, length);
    if (error) {
        std::cout << "Failed to send binary data" << std::endl;
    }
}

EM_BOOL OnMessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
    Connection* conn = (Connection*)userData;
    conn->last_received = (EmscriptenWebSocketMessageEvent*)websocketEvent;
    return EM_TRUE;
}

EM_BOOL OnOpen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData) {
    std::cout << "WebSocket connection opened" << std::endl;
    Connection* conn = (Connection*)userData;
    conn->connected = true;
    return EM_TRUE;
}