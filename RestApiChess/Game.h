#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "GameStatus.h"
#include "Move.h"
#include "Player.h"
#include <QList>
#include <QMap>

class Game {
public:
    Game(int id, Player *p1, Player *p2);

    int getGameId() const { return gameId; }
    Board* getBoard()  { return gameBoard; }
    ///
    /// \brief getPlayers
    /// \return Player Address and isWhite bool
    ///
    QMap<Player*, bool> getPlayers(){ return coleredPlayers; };

    Player *getFirstPlayer() const { return first; }
    Player *getSecondPlayer() const { return second; }

    bool isFirstPlayersTurn() const { return firstTurn; }
    GameStatus getStatus() const { return gameStatus; }
    const QList<Move*> getMoves() const { return gameMoves; }

    bool applyMove(Move *move, QString &error);
    void setResult(GameStatus status);

private:
    int gameId = 0;
    Board *gameBoard = new Board();
    Player *first = nullptr;
    Player *second = nullptr;
    QMap<Player*, bool> coleredPlayers = *new QMap<Player*, bool>();
    bool firstTurn = true;
    GameStatus gameStatus = GameStatus::InProgress;
    QList<Move*> gameMoves = *new QList<Move*>();
};

#endif // GAME_H
