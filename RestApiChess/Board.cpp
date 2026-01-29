#include "Board.h"
#include "Piece.h"

Board::Board() {
    // init positions
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            cells[y][x].pos = {x, y};
            cells[y][x].piece = nullptr;
        }
    }
    setupInitialPosition();
}

const Cell &Board::cellAt(Position *p) const {
    return cells[p->y][p->x];
}

Cell &Board::cellAt(Position *p) {
    return cells[p->y][p->x];
}

void Board::applyMove(const Move *move) {
    Cell &from = cellAt(move->from);
    Cell &to = cellAt(move->to);
    to.piece = from.piece;
    from.piece = nullptr;
}

bool Board::isMoveLegal(const Move *move, bool playerColor) const {
    const Cell &from = cellAt(move->from);
    if (!from.piece) return false;
    if (from.piece->isWhite() != playerColor) return false;
    return from.piece->canMove(*this, move->from, move->to);
}

void Board::setupInitialPosition() {
    // TODO: place pieces
}
