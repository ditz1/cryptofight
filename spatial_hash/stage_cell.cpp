#include "stage_cell.hpp"

StageCell::StageCell () {
    grid_pos = {0, 0};
    rect = {0, 0, 0, 0};
    collidable = false;
    color = GRAY;
}

void StageCell::Draw() {
    DrawRectangleRec(rect, color);
}