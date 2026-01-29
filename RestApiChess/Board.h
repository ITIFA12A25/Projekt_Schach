#ifndef BOARD_H
#define BOARD_H

using namespace std;

#include "Cell.h"
#include "Move.h"
#include "Player.h"
#include <array>

class Board {
public:
    Board();

    const Cell &cellAt(Position *p) const;
    Cell &cellAt(Position *p);

    void applyMove(const Move *move);
    bool isMoveLegal(const Move *move, bool whiteToMove) const;

private:
    array<array<Cell, 8>, 8> cells;
    void setupInitialPosition();
};

#endif // BOARD_H
