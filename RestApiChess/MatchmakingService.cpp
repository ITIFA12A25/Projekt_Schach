#include "MatchmakingService.h"
#include "Player.h"

MatchmakingService* MatchmakingService::instance = nullptr;

MatchmakingService* MatchmakingService::getInstance()
{
    if ( instance == nullptr )
        instance = new MatchmakingService;
    return instance;
}

Game *MatchmakingService::enqueuePlayer(Player *player) {
    if (!waiting.isEmpty()) {
        Player *other = waiting.dequeue();
        // assign colors arbitrarily
        Player *white = player;
        Player *black = other;

        int id = gameRepo->newGameId();
        Game *game = new Game(id, white, black);

        gameRepo->loadAll(userService->getPlayers());
        gameRepo->addGame(game);
        gameRepo->saveAll();

        return game;
    } else {
        waiting.enqueue(player);
        return nullptr;
    }
}

void MatchmakingService::cancelSearch(int playerId) {
    QQueue<Player*> tmp;
    while (!waiting.isEmpty()) {
        Player *p = waiting.dequeue();
        if (p->getId() != playerId)
            tmp.enqueue(p);
    }
    waiting = tmp;
}
