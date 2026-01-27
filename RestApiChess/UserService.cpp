#include "UserService.h"

UserService::UserService() {}

UserService* UserService::instance = nullptr;

UserService* UserService::getInstance()
{
    if ( instance == nullptr )
        instance = new UserService;
    return instance;
}

QList<Player*> UserService::getPlayers(){
    QList<Player*> players;
    persistence->loadPlayers(players, "players.json");
    return players;
}

Player* UserService::getPlayer(QString &playerName){
    QList<Player*> players;
    Player* searchPlayer = nullptr;
    players = getPlayers();

    foreach (auto p, players) {
        if(p->getName() == playerName)
            searchPlayer = p;
    }
    return searchPlayer;
}

Player* UserService::getPlayer(int &playerId){
    QList<Player*> players;
    Player* searchPlayer = nullptr;
    players = getPlayers();

    foreach (auto p, players) {
        if(p->getId() == playerId)
            searchPlayer = p;
    }
    return searchPlayer;
}

void UserService::savePlayer(Player *currentPlayer){
    QList<Player*> players;
    players = getPlayers();

    foreach (auto p, players) {
        if(p->getId() == currentPlayer->getId())
            p = currentPlayer;
    }
    persistence->savePlayers(players,"players.json");
}

void UserService::deletePlayer(Player *currentPlayer){
    QList<Player*> players;
    players = getPlayers();

            players.removeOne(currentPlayer);
    persistence->savePlayers(players,"players.json");
}

void UserService::registerPlayer(QString *playerName)
{
    QList<Player*> players;
    players = getPlayers();

    // Determine next available ID
    int nextId = 1;
    for (Player* p : players) {
        if (p->getId() >= nextId)
            nextId = p->getId() + 1;
    }

    // Create new player (default white = false)
    Player* newPlayer = new Player(nextId, *playerName, false);

    players.append(newPlayer);

    persistence->savePlayers(players, "players.json");
}

