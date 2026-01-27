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
        if (g->firstPlayer()->getId() == playerId ||
            g->secondPlayer()->getId() == playerId)
        {
            result.append(g);
        }
    }
    return result;
}

const Game *GameRepository::getGame(int gameId) const
{
    for (Game *g : games) {
        if (g->id() == gameId)
            return g;
    }
    return nullptr;
}
