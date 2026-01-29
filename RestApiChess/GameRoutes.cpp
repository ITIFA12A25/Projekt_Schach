#include "GameRoutes.h"

GameRoutes::GameRoutes(MoveValidator *validator)
    : validator(validator) {}

QList<ApiRoute> GameRoutes::registerRoutes() {
    QList<ApiRoute> routes;

    routes.append(ApiRoute{ HttpMethod::Get
                           , "/api/games"
                           , "List games for a player"
                           , [this](const QHttpServerRequest &req) { return listGames(req); }
                           , ""
                           , "GameListResponse"
                           , { {"playerId","integer",true} } });

    routes.append(ApiRoute{ HttpMethod::Get
                           , "/api/games/replay"
                           , "Get replay moves for a game"
                           , [this](const QHttpServerRequest &req) { return replay(req); }
                           , ""
                           , "GameReplayResponse"
                           , { {"gameId","integer",true} } });

    routes.append(ApiRoute{ HttpMethod::Post
                           , "/api/games/register"
                           , "Register a new game"
                           , [this](const QHttpServerRequest &req) { return registerGame(req); }
                           , ""
                           , "RegisterGameResponse"
                           , { {"firstPlayerId","integer",true}, {"secondPlayerId","integer",true} } });

    routes.append(ApiRoute{ HttpMethod::Post
                           , "/api/games/move"
                           , "Submit move for validation"
                           , [this](const QHttpServerRequest &req) { return move(req); }
                           , "MoveRequest"
                           , "MoveResponse"
                           , {} });
    return routes;
}

QHttpServerResponse GameRoutes::listGames(const QHttpServerRequest &req) {
    QUrlQuery q(req.url().query());
    bool ok = false;
    int playerId = q.queryItemValue("playerId").toInt(&ok);
    if (!ok) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Player ID missing"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    gameRepo->loadAll(userService->getPlayers());
    auto games = gameRepo->gamesForPlayer(playerId);
    QJsonArray arr;

    for (auto g : games) {
        QJsonObject o;
        o["gameId"] = g->getGameId();
        o["status"] = (int)g->getStatus();
        o["statusText"] = gameStatusToString(g->getStatus());
        o["firstPlayerId"] = g->getFirstPlayer()->getId();
        o["secondPlayerId"] = g->getSecondPlayer()->getId();
        o["firstPlayersTurn"] = g->isFirstPlayersTurn();
        o["moveCount"] = g->getMoves().size();
        o["isFinished"] = (g->getStatus() != GameStatus::InProgress);

        QJsonArray moves;
        for (const auto &m : g->getMoves()) {
            QJsonObject mo;
            mo["fromX"] = m->from->x;
            mo["fromY"] = m->from->y;
            mo["toX"] = m->to->x;
            mo["toY"] = m->to->y;
            mo["resign"] = m->resign;
            mo["drawOffer"] = m->drawOffer;
            moves.append(mo);
        }
        o["moves"] = moves;

        arr.append(o);
    }

    QJsonObject root;
    root["games"] = arr;

    return QHttpServerResponse("application/json",
                               QJsonDocument(root).toJson());
}

QHttpServerResponse GameRoutes::replay(const QHttpServerRequest &req) {
    QUrlQuery q(req.url().query());
    bool ok = false;
    int gameId = q.queryItemValue("gameId").toInt(&ok);
    if (!ok) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Game ID missing"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    const Game *game = gameRepo->getGame(gameId);

    if (!game) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Game not found"})",
                                   QHttpServerResponse::StatusCode::NotFound);
    }

    QJsonArray movesArr;
    for (const auto &m : game->getMoves()) {
        QJsonObject o;
        o["fromX"] = m->from->x;
        o["fromY"] = m->from->y;
        o["toX"] = m->to->x;
        o["toY"] = m->to->y;
        o["resign"] = m->resign;
        o["drawOffer"] = m->drawOffer;
        movesArr.append(o);
    }

    QJsonObject root;
    root["gameId"] = game->getGameId();
    root["moves"] = movesArr;

    return QHttpServerResponse("application/json",
                               QJsonDocument(root).toJson());
}

QHttpServerResponse GameRoutes::registerGame(const QHttpServerRequest &req) {
    QUrlQuery q(req.url().query());
    bool ok1 = false;
    bool ok2 = false;
    int firstPlayerId = q.queryItemValue("firstPlayerId").toInt(&ok1);
    int secondPlayerId = q.queryItemValue("secondPlayerId").toInt(&ok2);

    if (!ok1 || !ok2) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Player ID missing"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    auto p1 = userService->getPlayer(firstPlayerId);
    auto p2 = userService->getPlayer(secondPlayerId);

    if (!p1 || !p2) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Unknown player(s)\"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    int nextId = 1;
    for (auto g : gameRepo->allGames()) {
        if (g->getGameId() >= nextId)
            nextId = g->getGameId() + 1;
    }

    Game *game = new Game(nextId, p1, p2);
    gameRepo->addGame(game);
    gameRepo->saveAll();

    QJsonObject resp;
    resp["gameId"] = game->getGameId();
    resp["firstPlayerId"] = firstPlayerId;
    resp["secondPlayerId"] = secondPlayerId;
    resp["status"] = (int)game->getStatus();

    return QHttpServerResponse("application/json",
                               QJsonDocument(resp).toJson());
}

QHttpServerResponse GameRoutes::move(const QHttpServerRequest &req) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(req.body(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Invalid JSON"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    QJsonObject o = doc.object();
    int gameId = o.find("gameId").value().toInt();
    int playerId = o.find("playerId").value().toInt();

    gameRepo->loadAll(userService->getPlayers());
    Game *game = gameRepo->getGame(gameId);
    if (!game) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Game not found"})",
                                   QHttpServerResponse::StatusCode::NotFound);
    }

    if (playerId != game->getFirstPlayer()->getId() &&
        playerId != game->getSecondPlayer()->getId()) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Player not part of this game"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    bool isFirstPlayersTurn = game->isFirstPlayersTurn();
    int expectedPlayerId = isFirstPlayersTurn
                               ? game->getFirstPlayer()->getId()
                               : game->getSecondPlayer()->getId();

    if (playerId != expectedPlayerId) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Not your turn"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    Move *move = nullptr;
    move->player = userService->getPlayer(playerId);
    move->from->x = o["fromX"].toInt();
    move->from->y = o["fromY"].toInt();
    move->to->x = o["toX"].toInt();
    move->to->y = o["toY"].toInt();
    move->resign = o["resign"].toBool(false);
    move->drawOffer = o["drawOffer"].toBool(false);

    QString error;
    bool ok = validator->validateAndApply(*game, move, error);

    QJsonObject resp;
    resp["valid"] = ok;

    if (!ok) {
        resp["error"] = error;
        return QHttpServerResponse("application/json",
                                   QJsonDocument(resp).toJson());
    }

    gameRepo->saveAll();

    resp["status"] = (int)game->getStatus();
    resp["statusText"] = gameStatusToString(game->getStatus());

    return QHttpServerResponse("application/json",
                               QJsonDocument(resp).toJson());
}

