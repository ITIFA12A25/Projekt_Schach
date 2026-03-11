#include "Knight.h"
#include "Board.h"

bool Knight::canMove(const Board &board, Position *from, Position *to) const {
    int dx = qAbs(to->x - from->x);
    int dy = qAbs(to->y - from->y);
    if (!((dx == 1 && dy == 2) || (dx == 2 && dy == 1))) return false;

    const Cell &dest = board.cellAt(to);
    const Cell &src  = board.cellAt(from);
    if (!dest.isFilled() && src.isFilled()) return true;
    return  dest.isFilled() && src.isFilled() && dest.piece->isWhite() != src.piece->isWhite();
}
