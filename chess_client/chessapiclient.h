#ifndef CHESSAPICLIENT_H
#define CHESSAPICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonObject>

class ChessApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ChessApiClient(QObject *parent = nullptr);

    void getGames(int playerId);
    void getBoard(int gameId);
    void sendMove(const QJsonObject &move);
    void registerGame(int firstPlayerId, int secondPlayerId);
    void getReplay(int gameId);
    void matchmaking(int playerId);
    void registerPlayer(const QString &name);

signals:
    void gamesReceived(const QJsonObject &json);
    void boardReceived(const QJsonObject &json);
    void moveResponse(const QJsonObject &json);
    void gameRegistered(const QJsonObject &json);
    void replayReceived(const QJsonObject &json);
    void matchmakingResult(const QJsonObject &json);
    void playerRegistered(const QJsonObject &json);
    void errorReceived(const QString &op, const QString &message, const QJsonObject &json);

private slots:
    void onReply(QNetworkReply *reply);

private:
    QNetworkAccessManager manager;

    void get(const QString &op, const QString &path, const QUrlQuery &query);
    void post(const QString &op, const QString &path, const QJsonObject &body, const QUrlQuery &query = {});
};

#endif // CHESSAPICLIENT_H
