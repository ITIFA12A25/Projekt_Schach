#include "GameRepository.h"

GameRepository* GameRepository::instance = nullptr;

GameRepository::GameRepository() {}

GameRepository* GameRepository::getInstance()
{
    if (!instance)
        instance = new GameRepository;
    return instance;
}

void GameRepository::addGame(Game *game)
{
    games.append(game);
}

void GameRepository::saveAll()
{
    persistance->saveGames(games, "games.json");
}

void GameRepository::loadAll(const QList<Player*> &players)
{
    // Clean old games
    for (auto g : games)
        delete g;
    games.clear();

    QList<Game*> loaded;
    if (!persistance->loadGames(loaded, players, "games.json")) {
        qWarning() << "Failed to load games from" << "games.json";
        return;
    }

    games = loaded;
}

QList<const Game*> GameRepository::gamesForPlayer(int playerId) const
{
    QList<const Game*> result;
    for (Game *g : games) {
        if (g->getFirstPlayer()->getId() == playerId ||
            g->getSecondPlayer()->getId() == playerId)
        {
            result.append(g);
        }
    }
    return result;
}

Game *GameRepository::getGame(int gameId)
{
    persistance->loadGames(games, userService->getPlayers(), "games.json");
    for (Game *g : games) {
        if (g->getGameId() == gameId)
            return g;
    }
    return nullptr;
}

int GameRepository::newGameId() {
    persistance->loadGames(games, userService->getPlayers(), "games.json");

    int maxId = 0;

    for (const Game* g : games) {
        if (g && g->getGameId() > maxId) {
            maxId = g->getGameId();
        }
    }

    return maxId + 1;
}

