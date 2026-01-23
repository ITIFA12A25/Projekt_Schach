#ifndef PIECE_H
#define PIECE_H

#include "Position.h"

enum class Rank {
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

class Board;

class Piece {
public:
    Piece(bool white, Rank rank) : white(white), rank(rank), killed(false) {}
    virtual ~Piece() = default;

    virtual bool canMove(const Board &board, Position from, Position to) const = 0;

    Rank getRank() const { return rank; }
    bool isWhite() const { return white; }
    bool isKilled() const { return killed; }
    void setKilled(bool k) { killed = k; }

protected:
    bool white;
    Rank rank;
    bool killed;
};

#endif // PIECE_H
