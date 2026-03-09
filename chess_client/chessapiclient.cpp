#include "chessapiclient.h"

#include <QJsonDocument>
#include <QNetworkRequest>
#include <QUrl>

ChessApiClient::ChessApiClient(QObject *parent)
    : QObject(parent)
{
    connect(&manager, &QNetworkAccessManager::finished,
            this, &ChessApiClient::onReply);
}

void ChessApiClient::get(const QString &op, const QString &path, const QUrlQuery &query)
{
    QUrl url("http://localhost:8080" + path);
    url.setQuery(query);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setAttribute(QNetworkRequest::User, op);

    manager.get(req);
}

void ChessApiClient::post(const QString &op, const QString &path, const QJsonObject &body, const QUrlQuery &query)
{
    QUrl url("http://localhost:8080" + path);
    url.setQuery(query);

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setAttribute(QNetworkRequest::User, op);

    QJsonDocument doc(body);
    manager.post(req, doc.toJson(QJsonDocument::Compact));
}

void ChessApiClient::getGames(int playerId)
{
    QUrlQuery q;
    q.addQueryItem("playerId", QString::number(playerId));
    get("games", "/api/games", q);
}

void ChessApiClient::getBoard(int gameId)
{
    QUrlQuery q;
    q.addQueryItem("gameId", QString::number(gameId));
    get("board", "/api/games/board", q);
}

void ChessApiClient::sendMove(const QJsonObject &move)
{
    // Server expects strings for ints/bools
    QJsonObject body;
    body["gameId"]   = QString::number(move["gameId"].toInt());
    body["playerId"] = QString::number(move["playerId"].toInt());
    body["fromX"]    = QString::number(move["fromX"].toInt());
    body["fromY"]    = QString::number(move["fromY"].toInt());
    body["toX"]      = QString::number(move["toX"].toInt());
    body["toY"]      = QString::number(move["toY"].toInt());
    body["resign"]   = QString::number(move["resign"].toBool() ? 1 : 0);
    body["drawOffer"]= QString::number(move["drawOffer"].toBool() ? 1 : 0);

    post("move", "/api/games/move", body);
}

void ChessApiClient::registerGame(int firstPlayerId, int secondPlayerId)
{
    QUrlQuery q;
    q.addQueryItem("firstPlayerId", QString::number(firstPlayerId));
    q.addQueryItem("secondPlayerId", QString::number(secondPlayerId));
    post("registerGame", "/api/games/register", {}, q);
}

void ChessApiClient::getReplay(int gameId)
{
    QUrlQuery q;
    q.addQueryItem("gameId", QString::number(gameId));
    get("replay", "/api/games/replay", q);
}

void ChessApiClient::matchmaking(int playerId)
{
    QUrlQuery q;
    q.addQueryItem("playerId", QString::number(playerId));
    post("matchmaking", "/api/matchmaking/search", {}, q);
}

void ChessApiClient::registerPlayer(const QString &name)
{
    QUrlQuery q;
    q.addQueryItem("name", name);
    post("registerPlayer", "/api/players/register", {}, q);
}

void ChessApiClient::onReply(QNetworkReply *reply)
{
    QString op = reply->request().attribute(QNetworkRequest::User).toString();
    QByteArray data = reply->readAll();

    QJsonObject json;
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject())
        json = doc.object();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorReceived(op, reply->errorString(), json);
        reply->deleteLater();
        return;
    }

    if (op == "games")          emit gamesReceived(json);
    else if (op == "board")     emit boardReceived(json);
    else if (op == "move")      emit moveResponse(json);
    else if (op == "registerGame") emit gameRegistered(json);
    else if (op == "replay")    emit replayReceived(json);
    else if (op == "matchmaking") emit matchmakingResult(json);
    else if (op == "registerPlayer") emit playerRegistered(json);

    reply->deleteLater();
}
