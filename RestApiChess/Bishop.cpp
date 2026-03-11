#include "Bishop.h"
#include "Board.h"

bool Bishop::canMove(const Board &board, Position *from, Position *to) const {
    int dx = to->x - from->x;
    int dy = to->y - from->y;
    if (qAbs(dx) != qAbs(dy)) return false;

    int stepX = dx > 0 ? 1 : -1;
    int stepY = dy > 0 ? 1 : -1;

    Position p;
    p.x = from->x;
    p.y = from->y;

    const Cell &dest = board.cellAt(to);
    const Cell &src  = board.cellAt(from);

    while (true) {
        p.x += stepX;
        p.y += stepY;
        if (p.x == to->x && p.y == to->y) break;
        if (board.cellAt(&p).isFilled()) return false;
    }

    if (!dest.isFilled() && src.isFilled()) return true;
    return dest.isFilled() && src.isFilled() && dest.piece->isWhite() != src.piece->isWhite();
}
