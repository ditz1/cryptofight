#pragma once
#include <s_protocol.hpp>
#include <cstdint>

typedef struct Rectangle {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} Rectangle;

typedef union U16ToU8 {
    uint16_t u;
    struct {
        uint8_t b0;
        uint8_t b1;
    } b;
} U16ToU8;


Rectangle PlayerPosToRect(Vector2int player_position, float scale, int player_width, int player_height);
bool RectRectCollision(Rectangle rect1, Rectangle rect2);

