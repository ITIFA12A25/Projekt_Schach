#ifndef KING_H
#define KING_H

#include <QtGlobal>
#include "Piece.h"

class King : public Piece {
public:
    King(bool white) : Piece(white, Rank::King) {}
    bool canMove(const Board &board, Position from, Position to) const override;
};

#endif // KING_H
