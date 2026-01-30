#ifndef GAMEREPOSITORY_H
#define GAMEREPOSITORY_H

#include "Game.h"
#include "Player.h"
#include "Persistence.h"
#include "UserService.h"
#include <QList>
#include <QString>

class GameRepository {
public:
    GameRepository();
    static GameRepository *getInstance(void);

    void addGame(Game *game);

    QList<const Game*> gamesForPlayer(int playerId);
    Game *getGame(int gameId);
    QList<Game*> &allGames() { return games; }
    void saveAll();
    int newGameId();

private:
    static GameRepository *instance;
    Persistence *persistance = Persistence::getInstance();
    UserService *userService = UserService::getInstance();
    QList<Game*> games = *new QList<Game*>;

    void loadAll(const QList<Player*> &players);
};

#endif // GAMEREPOSITORY_H
