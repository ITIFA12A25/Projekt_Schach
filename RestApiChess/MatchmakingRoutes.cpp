#include "MatchmakingRoutes.h"

QList<ApiRoute> MatchmakingRoutes::registerRoutes() {
    QList<ApiRoute> routes;
    routes.append(ApiRoute {HttpMethod::Post, "/api/matchmaking/search", "Enqueue player for matchmaking",
                           [this](const QHttpServerRequest &req) {
                               return matchPlayers(req);
                           },
                           {},
                           "MatchmakingResponse",
                           { {"playerId","integer",true} }});
    return routes;
}



QHttpServerResponse MatchmakingRoutes::matchPlayers(const QHttpServerRequest &req){
    QUrlQuery q(req.url().query());
    bool ok = false;
    int playerId = q.queryItemValue("playerId").toInt(&ok);

    if (!ok) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Player ID missing"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    Player *player = userService->getPlayer(playerId);
    if (!player) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Unknown player"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    Game *game = matchmaking->enqueuePlayer(player);
    if (!game) {
        return QHttpServerResponse("application/json",
                                   R"({"status":"waiting"})");
    }

    QJsonObject json;
    json["status"] = gameStatusToString(GameStatus::InProgress);
    json["gameId"] = game->id();
    json["whitePlayerId"] = game->firstPlayer()->getId();
    json["blackPlayerId"] = game->secondPlayer()->getId();

    return QHttpServerResponse("application/json", QJsonDocument(json).toJson());
}
