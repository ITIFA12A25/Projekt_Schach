#ifndef QUEEN_H
#define QUEEN_H

#include <QtGlobal>
#include "Piece.h"

class Queen : public Piece {
public:
    Queen(bool white) : Piece(white, Rank::Queen) {}
    bool canMove(const Board &board, Position from, Position to) const override;
};

#endif // QUEEN_H
