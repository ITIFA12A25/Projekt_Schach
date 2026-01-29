#ifndef PAWN_H
#define PAWN_H

#include <QtGlobal>
#include "Piece.h"

class Pawn : public Piece {
public:
    Pawn(bool white) : Piece(white, Rank::Pawn) {}

    bool canMove(const Board &board, Position *from, Position *to) const override;
};

#endif // PAWN_H
