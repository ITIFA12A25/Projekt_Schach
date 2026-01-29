#include "Queen.h"
#include "Board.h"
#include "Rook.h"
#include "Bishop.h"

bool Queen::canMove(const Board &board, Position *from, Position *to) const {
    Rook r(white);
    Bishop b(white);
    return r.canMove(board, from, to) || b.canMove(board, from, to);
}
