#include "Rook.h"
#include "Board.h"

bool Rook::canMove(const Board &board, Position *from, Position *to) const {
    int dx = to->x - from->x;
    int dy = to->y - from->y;
    if (dx != 0 && dy != 0)
        return false;

    int stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
    int stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

    Position *p = from;
    const Cell &dest = board.cellAt(to);
    const Cell  &src = board.cellAt(from);
    while (true) {
        p->x += stepX;
        p->y += stepY;
        if (p->x == to->x && p->y == to->y) break;
        if (board.cellAt(p).isFilled() && src.isFilled())
            return false;
    }

    if (!dest.isFilled() && src.isFilled())
        return true;
    return dest.isFilled() && src.isFilled() && dest.piece->isWhite() != src.piece->isWhite();
}
