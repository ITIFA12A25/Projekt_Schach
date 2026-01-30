#include "Pawn.h"
#include "Board.h"

bool Pawn::canMove(const Board &board, Position *from, Position *to) const {
    int dir = white ? -1 : 1;
    int dx = to->x - from->x;
    int dy = to->y - from->y;

    const Cell &dest = board.cellAt(to);
    const Cell &src  = board.cellAt(from);

    // forward move
    if (dx == 0 && dy == dir && !dest.isFilled()) {
        return true;
    }

    // double move from starting rank
    if (dx == 0 && dy == 2*dir && !dest.isFilled()) {
        if ((white && from->y == 6) || (!white && from->y == 1)) {
            // naive: not checking intermediate square
            return true;
        }
    }

    // capture
    if (qAbs(dx) == 1 && dy == dir && dest.isFilled() && dest.piece->isWhite() != white) {
        return true;
    }

    return false;
}
