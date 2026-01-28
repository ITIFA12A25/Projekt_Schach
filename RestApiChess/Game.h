#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "GameStatus.h"
#include "Move.h"
#include "Player.h"
#include <QList>

class Game {
public:
    Game(int id, Player *p1, Player *p2);

    int getGameId() const { return gameId; }
    const Board &getBoard() const { return gameBoard; }
    Board &getBoard() { return gameBoard; }

    Player *getFirstPlayer() const { return first; }
    Player *getSecondPlayer() const { return second; }

    bool isFirstPlayersTurn() const { return firstTurn; }
    GameStatus getStatus() const { return gameStatus; }
    const QList<Move> &getMoves() const { return gameMoves; }

    bool applyMove(const Move &move, QString &error);
    void setResult(GameStatus status);

private:
    int gameId;
    Board gameBoard;
    Player *first;
    Player *second;
    bool firstTurn = true;
    GameStatus gameStatus = GameStatus::InProgress;
    QList<Move> gameMoves;
};

#endif // GAME_H
