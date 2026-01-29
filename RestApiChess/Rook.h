#ifndef ROOK_H
#define ROOK_H

#include <QtGlobal>
#include "Piece.h"

class Rook : public Piece {
public:
    Rook(bool white) : Piece(white, Rank::Rook) {}
    bool canMove(const Board &board, Position *from, Position *to) const override;
};

#endif // ROOK_H
