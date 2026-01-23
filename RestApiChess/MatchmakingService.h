#ifndef MATCHMAKINGSERVICE_H
#define MATCHMAKINGSERVICE_H

#include "Player.h"
#include "Game.h"
#include <QQueue>
#include <QMap>

class MatchmakingService {
public:
    MatchmakingService();
    static MatchmakingService *getInstance(void);

    Game *enqueuePlayer(Player *player);
    void cancelSearch(int playerId);
    Game *getGame(int gameId) const;

private:
    static MatchmakingService *instance;

    QQueue<Player*> waiting;
    QMap<int, Game*> games;
    int nextGameId = 1;
};

#endif // MATCHMAKINGSERVICE_H
