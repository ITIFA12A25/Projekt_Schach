#include "MatchmakingService.h"
#include "Player.h"

MatchmakingService* MatchmakingService::instance = nullptr;

MatchmakingService::MatchmakingService(){

}

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
        // In real code, you'd construct players with color; here we assume they already have it.

        int id = nextGameId++;
        Game *game = new Game(id, white, black);
        games.insert(id, game);
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

Game *MatchmakingService::getGame(int gameId) const {
    return games.value(gameId, nullptr);
}
