#include "Knight.h"
#include "Board.h"

bool Knight::canMove(const Board &board, Position *from, Position *to) const {
    int dx = qAbs(to->x - from->x);
    int dy = qAbs(to->y - from->y);
    if (!((dx == 1 && dy == 2) || (dx == 2 && dy == 1))) return false;

    const Cell &dest = board.cellAt(to);
    if (!dest.isFilled()) return true;
    return dest.piece->isWhite() != white;
}
