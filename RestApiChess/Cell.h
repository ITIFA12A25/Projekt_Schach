#ifndef CELL_H
#define CELL_H

#include "Position.h"
#include "Piece.h"

struct Cell {
    Position pos;
    Piece *piece = nullptr;
    bool isFilled() const { return piece != nullptr; }
};

#endif // CELL_H
