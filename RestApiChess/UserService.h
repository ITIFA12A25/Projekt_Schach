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

    QList<Player*> getPlayers();
    Player* getPlayer(QString playerName);
    Player* getPlayer(int playerId);
    void savePlayer(Player *currentPlayer);
    void deletePlayer(Player *currentPlayer);
    void registerPlayer(QString *playerName);

private:
    static UserService *instance;
    Persistence *persistence = Persistence::getInstance();

    Player *player = nullptr;
};

#endif // USERSERVICE_H
