#include "MatchmakingService.h"
#include "HumanPlayer.h"

MatchmakingService* MatchmakingService::instance = nullptr;

MatchmakingService::MatchmakingService(){

}

MatchmakingService* MatchmakingService::getInstance()
{
    if ( instance == nullptr )
        instance = new MatchmakingService;
    return instance;
}

Game *MatchmakingService::enqueuePlayer(IPlayer *player) {
    if (!waiting.isEmpty()) {
        IPlayer *other = waiting.dequeue();
        // assign colors arbitrarily
        IPlayer *white = player;
        IPlayer *black = other;
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
    QQueue<IPlayer*> tmp;
    while (!waiting.isEmpty()) {
        IPlayer *p = waiting.dequeue();
        if (p->getId() != playerId)
            tmp.enqueue(p);
    }
    waiting = tmp;
}

Game *MatchmakingService::getGame(int gameId) const {
    return games.value(gameId, nullptr);
}
