#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "GameStatus.h"
#include "Move.h"
#include "IPlayer.h"
#include <QVector>

class Game {
public:
    Game(int id, IPlayer *p1, IPlayer *p2);

    int id() const { return gameId; }
    const Board &board() const { return gameBoard; }
    Board &board() { return gameBoard; }

    IPlayer *firstPlayer() const { return first; }
    IPlayer *secondPlayer() const { return second; }

    bool isFirstPlayersTurn() const { return firstTurn; }
    GameStatus status() const { return gameStatus; }
    const QVector<Move> &moves() const { return gameMoves; }

    bool applyMove(const Move &move, QString &error);
    void setResult(GameStatus status);

private:
    int gameId;
    Board gameBoard;
    IPlayer *first;
    IPlayer *second;
    bool firstTurn = true;
    GameStatus gameStatus = GameStatus::InProgress;
    QVector<Move> gameMoves;
};

#endif // GAME_H
