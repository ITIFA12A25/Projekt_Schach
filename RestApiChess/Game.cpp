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

    if (!gameBoard.isMoveLegal(move, coleredPlayers.constFind(move->player).value())) {
        error = "Illegal move";
        return false;
    }

    gameBoard.applyMove(move);
    gameMoves.append(move);
    firstTurn = !firstTurn;

    // TODO: check checkmate/stalemate and update gameStatus

    return true;
}

void Game::setResult(GameStatus status) {
    gameStatus = status;
}
