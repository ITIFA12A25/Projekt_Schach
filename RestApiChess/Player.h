#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player
{
public:
    Player(int id, const QString &name, bool white);
    bool isWhite();
    int getId();
    QString getName();
private:
    int idPlayer;
    QString namePlayer;
    bool whitePlayer;
};

#endif // PLAYER_H
