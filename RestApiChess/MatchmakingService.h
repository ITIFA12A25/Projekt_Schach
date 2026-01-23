#ifndef MATCHMAKINGSERVICE_H
#define MATCHMAKINGSERVICE_H

#include "IPlayer.h"
#include "Game.h"
#include <QQueue>
#include <QMap>

class MatchmakingService {
public:
    MatchmakingService();
    static MatchmakingService *getInstance(void);

    Game *enqueuePlayer(IPlayer *player);
    void cancelSearch(int playerId);
    Game *getGame(int gameId) const;

private:
    static MatchmakingService *instance;

    QQueue<IPlayer*> waiting;
    QMap<int, Game*> games;
    int nextGameId = 1;
};

#endif // MATCHMAKINGSERVICE_H
