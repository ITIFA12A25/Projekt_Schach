#include "Game.h"

Game::Game(int id, Player *p1, Player *p2)
    : gameId(id), first(p1), second(p2) {
    coleredPlayers.insert(first, true);
    coleredPlayers.insert(second, false);
}

bool Game::applyMove(Move *move, QString &error) {
    if (gameStatus != GameStatus::InProgress) {
        error = "Game is not in progress";
        return false;
    }
    bool playerColor = false;
     for (auto it = coleredPlayers.begin(); it != coleredPlayers.end(); ++it)
    {
        Player* p = it.key();
        if (p && p->getId() == move->player->getId()){
            playerColor = it.value();
        }
     }

    if (!gameBoard->isMoveLegal(move, playerColor)) {
        error = "Illegal move";
        return false;
    }

    gameBoard->applyMove(move);
    gameMoves.append(move);
    firstTurn = !firstTurn;

    // TODO: check checkmate/stalemate and update gameStatus

    return true;
}

void Game::setResult(GameStatus status) {
    gameStatus = status;
}
