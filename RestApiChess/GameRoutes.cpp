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
    routes.append(ApiRoute{ HttpMethod::Get
                           , "/api/games/board"
                           , "Get full board state for a game"
                           , [this](const QHttpServerRequest &req) { return boardState(req); }
                           , ""
                           , "GameBoardStateResponse"
                           , { {"gameId","integer",true} } });

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
    int gameId = o.value("gameId").toString().toInt();
    int playerId = o.value("playerId").toString().toInt();

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

    QJsonObject errors;

    // Helper for integer fields (positions)
    auto parseInt = [&](const QString& key, int& outValue) {
        bool ok = false;
        outValue = o.value(key).toString().toInt(&ok);
        if (!ok) {
            errors.insert(key, "Invalid integer");
        }
    };

    // Helper for boolean fields (must be 0 or 1)
    auto parseBool = [&](const QString& key, bool& outValue) {
        bool ok = false;
        int temp = o.value(key).toString().toInt(&ok);

        if (!ok || (temp != 0 && temp != 1)) {
            errors.insert(key, "Invalid boolean (expected 0 or 1)");
            return;
        }

        outValue = (temp == 1);
    };

    Move* move = new Move;
    move->player = userService->getPlayer(playerId);

    // Parse positions
    move->from = new Position;
    parseInt("fromX", move->from->x);
    parseInt("fromY", move->from->y);

    move->to = new Position;
    parseInt("toX", move->to->x);
    parseInt("toY", move->to->y);

    // Parse booleans
    parseBool("resign", move->resign);
    parseBool("drawOffer", move->drawOffer);

    // If any errors occurred, return detailed JSON
    if (!errors.isEmpty()) {
        QJsonObject response;
        response.insert("error", "Invalid JSON fields");
        response.insert("details", errors);

        QJsonDocument doc(response);
        return QHttpServerResponse(
            "application/json",
            doc.toJson(QJsonDocument::Compact),
            QHttpServerResponse::StatusCode::BadRequest
            );
    }

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

QHttpServerResponse GameRoutes::boardState(const QHttpServerRequest &req) {
    QUrlQuery q(req.url().query());
    bool ok = false;
    int gameId = q.queryItemValue("gameId").toInt(&ok);

    if (!ok) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Game ID missing"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    Game *game = gameRepo->getGame(gameId);
    if (!game) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Game not found"})",
                                   QHttpServerResponse::StatusCode::NotFound);
    }

    const Board *board = game->getBoard();
    if (!board) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Board not initialized"})",
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    QJsonArray positions;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Position *pos = new Position;
            pos->x = x;
            pos->y = y;
            const Cell &c = board->cellAt(pos);

            QJsonObject Jpos;
            Jpos["x"] = c.pos.x;
            Jpos["y"] = c.pos.y;

            if (c.piece) {
                QJsonObject p;
                p["type"] = rankToString(c.piece->getRank());
                p["white"] = c.piece->isWhite();
                Jpos["piece"] = p;
            } else {
                Jpos["piece"] = QJsonValue(); // null
            }

            positions.append(Jpos);
        }
    }

    QJsonObject root;
    root["gameId"] = gameId;
    root["positions"] = positions;
    root["firstPlayersTurn"] = game->isFirstPlayersTurn();
    root["status"] = (int)game->getStatus();
    root["statusText"] = gameStatusToString(game->getStatus());

    return QHttpServerResponse("application/json",
                               QJsonDocument(root).toJson());
}


