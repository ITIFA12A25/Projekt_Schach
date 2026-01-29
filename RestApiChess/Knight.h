#ifndef KNIGHT_H
#define KNIGHT_H

#include <QtGlobal>
#include "Piece.h"

class Knight : public Piece {
public:
    Knight(bool white) : Piece(white, Rank::Knight) {}
    bool canMove(const Board &board, Position *from, Position *to) const override;
};

#endif // KNIGHT_H
