#ifndef BISHOP_H
#define BISHOP_H

#include <QtGlobal>
#include "Piece.h"

class Bishop : public Piece {
public:
    Bishop(bool white) : Piece(white, Rank::Bishop) {}
    bool canMove(const Board &board, Position from, Position to) const override;
};

#endif // BISHOP_H
