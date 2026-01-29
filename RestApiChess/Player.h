#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player
{
public:
    Player(int id, const QString &name, bool white);
    int getId();
    QString getName();
private:
    int idPlayer;
    QString namePlayer;
};

#endif // PLAYER_H
