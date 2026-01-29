#ifndef MOVE_H
#define MOVE_H

#include "Position.h"
#include "Player.h"

struct Move {
    Player *player = nullptr;
    Position *from = nullptr;
    Position *to = nullptr;
    bool resign = false;
    bool drawOffer = false;
};

#endif // MOVE_H
