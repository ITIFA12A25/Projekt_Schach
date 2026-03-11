#include "Queen.h"
#include "Board.h"
#include "Rook.h"
#include "Bishop.h"

bool Queen::canMove(const Board &board, Position *from, Position *to) const {
    Rook r(white);
    Bishop b(white);
    bool canMove = false;
    bool canUseRookMoveSet =  r.canMove(board, from, to);
        bool canUseBishopMoveSet = b.canMove(board, from, to);
    if (canUseBishopMoveSet && !canUseRookMoveSet){
        canMove = true;
    }
    else if (!canUseBishopMoveSet && canUseRookMoveSet){
        canMove = true;
    }
    return canMove;
}
