#ifndef GAMEREPOSITORY_H
#define GAMEREPOSITORY_H

#include "Game.h"
#include "HumanPlayer.h"
#include "Persistence.h"
#include <QVector>
#include <QString>

class GameRepository {
public:
    GameRepository();
    static GameRepository *getInstance(void);

    void addGame(Game *game);
    void saveAll(const QString &filePath);
    void loadAll(const QString &filePath, const QVector<HumanPlayer*> &players);

    QVector<const Game*> gamesForPlayer(int playerId) const;
    const Game *getGame(int gameId) const;
    QVector<Game*> &allGames() { return games; }

private:
    static GameRepository *instance;
    Persistence *persistance = Persistence::getInstance();
    QVector<Game*> games;
};

#endif // GAMEREPOSITORY_H
