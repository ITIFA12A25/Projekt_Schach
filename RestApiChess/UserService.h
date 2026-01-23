#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "Persistence.h"
#include "IPlayer.h"
#include "HumanPlayer.h"
#include <QString>
#include <QVector>

class UserService
{
public:
    UserService();
    static UserService *getInstance(void);

    HumanPlayer* getPlayer(QString &playerName);
    void savePlayer(HumanPlayer *currentPlayer);
    void deletePlayer(HumanPlayer *currentPlayer);
    void registerPlayer(HumanPlayer *newPlayer);

private:
    static UserService *instance;
    Persistence *persistence = Persistence::getInstance();

    IPlayer *player = nullptr;
};

#endif // USERSERVICE_H
