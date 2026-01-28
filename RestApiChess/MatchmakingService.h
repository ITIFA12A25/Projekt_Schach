#ifndef MATCHMAKINGSERVICE_H
#define MATCHMAKINGSERVICE_H

#include "Player.h"
#include "GameRepository.h"
#include "UserService.h"
#include "Game.h"
#include <QQueue>
#include <QMap>

class MatchmakingService {
public:
    static MatchmakingService *getInstance(void);

    Game *enqueuePlayer(Player *player);
    void cancelSearch(int playerId);
    Game *getGame(int gameId) const;

private:
    static MatchmakingService *instance;

    GameRepository *gameRepo = GameRepository::getInstance();
    UserService *userService = UserService::getInstance();
    QQueue<Player*> waiting;
    int nextGameId = 1;
};

#endif // MATCHMAKINGSERVICE_H
