#include "Board.h"

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
    // Place pawns
    for (int x = 0; x < 8; ++x) {
        cells[1][x].piece = new Pawn(false);
        // Black pawns
        cells[6][x].piece = new Pawn(true);
        // White pawns
    }
    // Place rooks
    cells[0][0].piece = new Rook(false);
    cells[0][7].piece = new Rook(false);
    cells[7][0].piece = new Rook(true);
    cells[7][7].piece = new Rook(true);
    // Place knights
    cells[0][1].piece = new Knight(false);
    cells[0][6].piece = new Knight(false);
    cells[7][1].piece = new Knight(true);
    cells[7][6].piece = new Knight(true);
    // Place bishops
    cells[0][2].piece = new Bishop(false);
    cells[0][5].piece = new Bishop(false);
    cells[7][2].piece = new Bishop(true);
    cells[7][5].piece = new Bishop(true);
    // Place queens
    cells[0][3].piece = new Queen(false);
    cells[7][3].piece = new Queen(true);
    // Place kings
    cells[0][4].piece = new King(false);
    cells[7][4].piece = new King(true);
}
