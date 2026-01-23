#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "GameStatus.h"
#include "Move.h"
#include "Player.h"
#include <QVector>

class Game {
public:
    Game(int id, Player *p1, Player *p2);

    int id() const { return gameId; }
    const Board &board() const { return gameBoard; }
    Board &board() { return gameBoard; }

    Player *firstPlayer() const { return first; }
    Player *secondPlayer() const { return second; }

    bool isFirstPlayersTurn() const { return firstTurn; }
    GameStatus status() const { return gameStatus; }
    const QVector<Move> &moves() const { return gameMoves; }

    bool applyMove(const Move &move, QString &error);
    void setResult(GameStatus status);

private:
    int gameId;
    Board gameBoard;
    Player *first;
    Player *second;
    bool firstTurn = true;
    GameStatus gameStatus = GameStatus::InProgress;
    QVector<Move> gameMoves;
};

#endif // GAME_H
