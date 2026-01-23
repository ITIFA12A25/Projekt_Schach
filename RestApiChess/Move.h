#ifndef MOVE_H
#define MOVE_H

#include "Position.h"

struct Move {
    Position from;
    Position to;
    bool resign = false;
    bool drawOffer = false;
};

#endif // MOVE_H
