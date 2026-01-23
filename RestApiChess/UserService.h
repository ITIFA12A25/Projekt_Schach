#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "Persistence.h"
#include "Player.h"
#include <QString>
#include <QVector>

class UserService
{
public:
    UserService();
    static UserService *getInstance(void);

    Player* getPlayer(QString &playerName);
    void savePlayer(Player *currentPlayer);
    void deletePlayer(Player *currentPlayer);
    void registerPlayer(Player *newPlayer);

private:
    static UserService *instance;
    Persistence *persistence = Persistence::getInstance();

    Player *player = nullptr;
};

#endif // USERSERVICE_H
