#include "Game.h"

Game::Game(int id, IPlayer *p1, IPlayer *p2)
    : gameId(id), first(p1), second(p2) {}

bool Game::applyMove(const Move &move, QString &error) {
    if (gameStatus != GameStatus::InProgress) {
        error = "Game is not in progress";
        return false;
    }

    bool whiteToMove = firstTurn ? first->isWhite() : second->isWhite();
    if (!gameBoard.isMoveLegal(move, whiteToMove)) {
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
