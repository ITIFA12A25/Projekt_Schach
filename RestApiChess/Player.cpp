#include "Player.h"

Player::Player(int id, const QString &name, bool white)
    : idPlayer(id), namePlayer(name), whitePlayer(white){

}

bool Player::isWhite() {
    return whitePlayer;
}

int Player::getId() {
    return idPlayer;
}

QString Player::getName() {
    return namePlayer;
}
