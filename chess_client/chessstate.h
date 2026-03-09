#ifndef CHESSSTATE_H
#define CHESSSTATE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class ChessState : public QObject
{
    Q_OBJECT

public:
    explicit ChessState(QObject *parent = nullptr);

    QJsonArray games;
    QJsonObject board;
    QJsonArray replayMoves;
    QJsonObject lastMoveResponse;
    QJsonObject matchmakingInfo;
    QJsonObject registeredPlayer;

    bool hasSelection = false;
    int selX = -1;
    int selY = -1;

signals:
    void stateUpdated();
};

#endif // CHESSSTATE_H
