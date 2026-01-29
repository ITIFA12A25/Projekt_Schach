#ifndef MOVEVALIDATOR_H
#define MOVEVALIDATOR_H

#include "Game.h"
#include <QString>

class MoveValidator {
public:
    bool validateAndApply(Game &game, Move *move, QString &error) {
        return game.applyMove(move, error);
    }
};

#endif // MOVEVALIDATOR_H
