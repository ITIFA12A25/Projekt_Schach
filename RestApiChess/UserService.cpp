#include "UserService.h"

UserService::UserService() {}

UserService* UserService::instance = nullptr;

UserService* UserService::getInstance()
{
    if ( instance == nullptr )
        instance = new UserService;
    return instance;
}

Player* UserService::getPlayer(QString &playerName){
    QList<Player*> players;
    Player* searchPlayer = nullptr;
    persistence->loadPlayers(players, "players.json");

    foreach (auto p, players) {
        if(p->getName() == playerName)
            searchPlayer = p;
    }
    return searchPlayer;
}

void UserService::savePlayer(Player *currentPlayer){
    QList<Player*> players;
    persistence->loadPlayers(players, "players.json");

    foreach (auto p, players) {
        if(p->getId() == currentPlayer->getId())
            p = currentPlayer;
    }
    persistence->savePlayers(players,"players.json");
}

void UserService::deletePlayer(Player *currentPlayer){
    QList<Player*> players;
    persistence->loadPlayers(players, "players.json");

            players.removeOne(currentPlayer);
    persistence->savePlayers(players,"players.json");
}

void UserService::registerPlayer(Player *newPlayer){

}
