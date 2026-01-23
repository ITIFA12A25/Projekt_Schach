#include "GameRepository.h"

GameRepository* GameRepository::instance = nullptr;

GameRepository::GameRepository(){

}

GameRepository* GameRepository::getInstance()
{
    if ( instance == nullptr )
        instance = new GameRepository;
    return instance;
}

void GameRepository::addGame(Game *game) {
    games.append(game);
}

void GameRepository::saveAll(const QString &filePath) {
    persistance->saveGames(games, filePath);
}

void GameRepository::loadAll(const QString &filePath, const QVector<Player*> &players) {
    QVector<Game*> loaded;
    persistance->loadGames(loaded, players, filePath);
    games = loaded;
}

QVector<const Game*> GameRepository::gamesForPlayer(int playerId) const {
    QVector<const Game*> result;
    for (Game *g : games) {
        if (g->firstPlayer()->getId() == playerId ||
            g->secondPlayer()->getId() == playerId) {
            result.append(g);
        }
    }
    return result;
}

const Game *GameRepository::getGame(int gameId) const {
    for (Game *g : games) {
        if (g->id() == gameId) return g;
    }
    return nullptr;
}
