#ifndef GAMEREPOSITORY_H
#define GAMEREPOSITORY_H

#include "Game.h"
#include "Player.h"
#include "Persistence.h"
#include <QList>
#include <QString>

class GameRepository {
public:
    GameRepository();
    static GameRepository *getInstance(void);

    void addGame(Game *game);
    void saveAll();
    void loadAll(const QList<Player*> &players);

    QList<const Game*> gamesForPlayer(int playerId) const;
    const Game *getGame(int gameId) const;
    QList<Game*> &allGames() { return games; }

private:
    static GameRepository *instance;
    Persistence *persistance = Persistence::getInstance();
    QList<Game*> games;
};

#endif // GAMEREPOSITORY_H
