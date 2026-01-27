#include "apiserver.h"
#include <QFile>
#include <QDebug>

ApiServer::ApiServer() {}

ApiServer* ApiServer::instance = nullptr;


ApiServer* ApiServer::getInstance()
{
    if ( instance == nullptr )
        instance = new ApiServer;
    return instance;
}


void ApiServer::Start(QCoreApplication &app) {
    getServerPort(app);

    qDebug() << "Server running on port:" << portArg;

    setupSchemas();
    qDebug() << "Schemas registered:" << schemaList.size();

    setupRoutes();
    qDebug() << "Routes registered:" << routeList.size();

    setServer(app);
    qDebug() << "HTTP server started.";

    qDebug().noquote() << "REST API Endpoint Docs:\t" << "http://" + hostName.toString() + ":" + QString::number(portArg) + "/rest-ui";
    // On shutdown call:
    // gameRepo->saveAll("games.json");
    // Persistence::savePlayers(playerVec, "players.json");
}

void ApiServer::registerSchema(const QString &name, const QJsonObject &schema) {
    schemaList.append(ApiSchema{name, schema});
}

void ApiServer::registerRoute(
    const QString &method,
    const QString &path,
    const QString &description,
    function<QHttpServerResponse(const QHttpServerRequest&)> handler,
    const QString &requestSchema,
    const QString &responseSchema,
    const QList<ApiQueryParam> &queryParams
    ) {
    routeList.append(ApiRoute{method, path, description, requestSchema, responseSchema, queryParams});

    auto wrapper = [handler](const QHttpServerRequest &req) {
        return handler(req);
    };

    if (method == "GET") {
        server.route(path, wrapper);
    } else if (method == "POST") {
        server.route(path, QHttpServerRequest::Method::Post, wrapper);
    } else if (method == "PUT") {
        server.route(path, QHttpServerRequest::Method::Put, wrapper);
    } else if (method == "DELETE") {
        server.route(path, QHttpServerRequest::Method::Delete, wrapper);
    }
}

void ApiServer::setupRoutes() {
    //
    // ────────────────────────────────────────────────────────────────
    //  ROUTES
    // ────────────────────────────────────────────────────────────────
    //

    registerRoute("GET", "/", "Root endpoint",
                  [](const QHttpServerRequest &) {
                      return QHttpServerResponse("text/plain", "Hello from Qt REST API!");
                  }
                  );

    registerRoute("GET", "/hello", "Simple hello endpoint",
                  [](const QHttpServerRequest &) {
                      return QHttpServerResponse("text/plain", "Hello from Qt REST API!");
                  }
                  );

    registerRoute("GET", "/sum", "Calculate sum of a and b",
                  [](const QHttpServerRequest &req) {
                      QUrlQuery q(req.url().query());
                      bool okA, okB;
                      int a = q.queryItemValue("a").toInt(&okA);
                      int b = q.queryItemValue("b").toInt(&okB);

                      QJsonObject json;
                      if (!okA || !okB) {
                          json["error"] = "Missing or invalid parameters";
                          return QHttpServerResponse("application/json",
                                                     QJsonDocument(json).toJson(),
                                                     QHttpServerResponse::StatusCode::BadRequest);
                      }

                      json["a"] = a;
                      json["b"] = b;
                      json["sum"] = a + b;

                      return QHttpServerResponse("application/json",
                                                 QJsonDocument(json).toJson());
                  },
                  {},
                  {},
                  { {"a","integer",true}, {"b","integer",true} }
                  );

    //
    // ────────────────────────────────────────────────────────────────
    //  MATCHMAKING
    // ────────────────────────────────────────────────────────────────
    //

    registerRoute("POST", "/api/matchmaking/search", "Enqueue player for matchmaking",
        [this](const QHttpServerRequest &req) {
            QUrlQuery q(req.url().query());
            bool ok = false;
            int playerId = q.queryItemValue("playerId").toInt(&ok);
            if (!ok) {
                return QHttpServerResponse("application/json", R"({"error":"Player ID missing"})", QHttpServerResponse::StatusCode::BadRequest);
            }


            QList<Player*> players;
            Player *player =userService->getPlayer(playerId);
            qDebug() << player;

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

            return QHttpServerResponse("application/json",
                                       QJsonDocument(json).toJson());
        },
        {},
        "MatchmakingResponse",
        { {"playerId","integer",true}}
        );

    //
    // ────────────────────────────────────────────────────────────────
    //  GAME HANDELING
    // ────────────────────────────────────────────────────────────────
    //

    registerRoute("GET", "/api/games", "List games for a player",
        [this](const QHttpServerRequest &req) {
            QUrlQuery q(req.url().query());
            bool ok = false;
            int playerId = q.queryItemValue("playerId").toInt(&ok);
            if (!ok) {
                return QHttpServerResponse("application/json", R"({"error":"Player ID missing"})", QHttpServerResponse::StatusCode::BadRequest);
            }

            gameRepo->loadAll(userService->getPlayers());
            auto games = gameRepo->gamesForPlayer(playerId);
            QJsonArray arr;

            for (auto g : games) {
                QJsonObject o;
                o["gameId"] = g->id();
                o["status"] = (int)g->status();
                o["statusText"] = gameStatusToString(g->status());
                o["firstPlayerId"] = g->firstPlayer()->getId();
                o["secondPlayerId"] = g->secondPlayer()->getId();
                o["firstPlayersTurn"] = g->isFirstPlayersTurn();
                o["moveCount"] = g->moves().size();
                o["isFinished"] = (g->status() != GameStatus::InProgress);

                QJsonArray moves;
                for (const auto &m : g->moves()) {
                    QJsonObject mo;
                    mo["fromX"] = m.from.x;
                    mo["fromY"] = m.from.y;
                    mo["toX"] = m.to.x;
                    mo["toY"] = m.to.y;
                    mo["resign"] = m.resign;
                    mo["drawOffer"] = m.drawOffer;
                    moves.append(mo);
                }
                o["moves"] = moves;

                arr.append(o);
            }

            QJsonObject root;
            root["games"] = arr;

            return QHttpServerResponse("application/json",
                                       QJsonDocument(root).toJson());
        },
        {}, // no request schema
        "GameListResponse", // response schema
        { {"playerId","integer",true} } // query param schema
        );

    registerRoute("GET", "/api/games/replay", "Get replay moves for a game",
        [this](const QHttpServerRequest &req) {
            QUrlQuery q(req.url().query());
            bool ok = false;
            int gameId = q.queryItemValue("gameId").toInt(&ok);
            if (!ok) {
                return QHttpServerResponse("application/json", R"({"error":"Game ID missing"})", QHttpServerResponse::StatusCode::BadRequest);
            }

            const Game *game = gameRepo->getGame(gameId);

            if (!game) {
                return QHttpServerResponse("application/json", R"({"error":"Game not found"})", QHttpServerResponse::StatusCode::NotFound);
            }

            QJsonArray movesArr;
            for (const auto &m : game->moves()) {
                QJsonObject o;
                o["fromX"] = m.from.x;
                o["fromY"] = m.from.y;
                o["toX"] = m.to.x;
                o["toY"] = m.to.y;
                o["resign"] = m.resign;
                o["drawOffer"] = m.drawOffer;
                movesArr.append(o);
            }

            QJsonObject root;
            root["gameId"] = game->id();
            root["moves"] = movesArr;

            return QHttpServerResponse("application/json",
                                       QJsonDocument(root).toJson());
        },
        {}, // no request schema
        "GameReplayResponse", // response schema
        { {"gameId","integer",true} } // query param schema
    );

    registerRoute("POST", "/api/games/register", "Register a new game",
        [this](const QHttpServerRequest &req) {
            QUrlQuery q(req.url().query());
            bool ok1 = false;
            bool ok2 = false;
            int firstPlayerId = q.queryItemValue("firstPlayerId").toInt(&ok1);
            int secondPlayerId = q.queryItemValue("secondPlayerId").toInt(&ok2);

            if (!ok1 || !ok2) {
                return QHttpServerResponse("application/json", R"({"error":"Player ID missing"})", QHttpServerResponse::StatusCode::BadRequest);
            }

            auto p1 = userService->getPlayer(firstPlayerId);
            auto p2 = userService->getPlayer(secondPlayerId);

            if (!p1 || !p2) {
                return QHttpServerResponse("application/json", R"({"error":"Unknown player(s)\"})", QHttpServerResponse::StatusCode::BadRequest);
            }

            // Determine next game ID
            int nextId = 1;
            for (auto g : gameRepo->allGames()) {
                if (g->id() >= nextId)
                    nextId = g->id() + 1;
            }

            // Create game
            Game *game = new Game(nextId, p1, p2);
            gameRepo->addGame(game);
            gameRepo->saveAll();

            // Response
            QJsonObject resp;
            resp["gameId"] = game->id();
            resp["firstPlayerId"] = firstPlayerId;
            resp["secondPlayerId"] = secondPlayerId;
            resp["status"] = (int)game->status();

            return QHttpServerResponse("application/json",
                                       QJsonDocument(resp).toJson());
        },
        {},
        "RegisterGameResponse",
        { {"firstPlayerId","string",true}, {"secondPlayerId","string",true} }
    );

    registerRoute("POST", "/api/games/move", "Submit move for validation",
        [this](const QHttpServerRequest &req) {
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(req.body(), &err);
            if (err.error != QJsonParseError::NoError || !doc.isObject()) {
                return QHttpServerResponse("application/json",
                                           R"({"error":"Invalid JSON"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }

            QJsonObject o = doc.object();
            int gameId = o["gameId"].toInt();
            int playerId = o["playerId"].toInt();

            gameRepo->loadAll(userService->getPlayers());
            Game *game = gameRepo->getGame(gameId);
            if (!game) {
                return QHttpServerResponse("application/json",
                                           R"({"error":"Game not found"})",
                                           QHttpServerResponse::StatusCode::NotFound);
            }

            // 1. Check if player belongs to this game
            if (playerId != game->firstPlayer()->getId() &&
                playerId != game->secondPlayer()->getId()) {
                return QHttpServerResponse("application/json",
                                           R"({"error":"Player not part of this game"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }

            // 2. Check if it is the player's turn
            bool isFirstPlayersTurn = game->isFirstPlayersTurn();
            int expectedPlayerId = isFirstPlayersTurn
                                       ? game->firstPlayer()->getId()
                                       : game->secondPlayer()->getId();

            if (playerId != expectedPlayerId) {
                return QHttpServerResponse("application/json",
                                           R"({"error":"Not your turn"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }

            // 3. Build move
            Move move;
            move.from = { o["fromX"].toInt(), o["fromY"].toInt() };
            move.to   = { o["toX"].toInt(),   o["toY"].toInt() };
            move.resign = o["resign"].toBool(false);
            move.drawOffer = o["drawOffer"].toBool(false);

            // 4. Validate + apply move
            QString error;
            bool ok = moveValidator.validateAndApply(*game, move, error);

            QJsonObject resp;
            resp["valid"] = ok;

            if (!ok) {
                resp["error"] = error;
                return QHttpServerResponse("application/json",
                                           QJsonDocument(resp).toJson());
            }

            // 5. Save updated game state
            gameRepo->saveAll();

            resp["status"] = (int)game->status();
            resp["statusText"] = gameStatusToString(game->status());

            return QHttpServerResponse("application/json",
                                       QJsonDocument(resp).toJson());
        },
        "MoveRequest",
        "MoveResponse"
    );


    //
    // ────────────────────────────────────────────────────────────────
    //  USER HANDELING
    // ────────────────────────────────────────────────────────────────
    //

    registerRoute("POST", "/api/players/register", "Register a new player",
                  [this](const QHttpServerRequest &req) {
                      QUrlQuery q(req.url().query());

                      string newPlayerName = q.queryItemValue("name").toStdString();

                      if (newPlayerName.empty()) {
                          return QHttpServerResponse("application/json", R"({"error":"New player name"})", QHttpServerResponse::StatusCode::BadRequest);
                      }

                      QString name = QString::fromStdString(newPlayerName);

                      if (name.isEmpty()) {
                          return QHttpServerResponse("application/json",
                                                     R"({"error":"Missing 'name'"})",
                                                     QHttpServerResponse::StatusCode::BadRequest);
                      }

                      // Call your service
                      userService->registerPlayer(&name);

                      Player* created = userService->getPlayer(name);

                      QJsonObject resp;
                      resp["id"] = created->getId();
                      resp["name"] = created->getName();
                      resp["white"] = created->isWhite();

                      return QHttpServerResponse("application/json", QJsonDocument(resp).toJson());
                  },
                  {},
                  "RegisterPlayerResponse",
                  { {"name","string",true} }
                  );


    //
    // ────────────────────────────────────────────────────────────────
    //  OPENAPI JSON & REST UI
    // ────────────────────────────────────────────────────────────────
    //

    registerRoute("GET", "/openapi.json", "OpenAPI 3.0 specification",
        [this](const QHttpServerRequest &) {

            QJsonObject openapi;
            openapi["openapi"] = "3.0.0";

            QJsonObject info;
            info["title"] = "Qt Chess API";
            info["version"] = "1.0.0";
            openapi["info"] = info;

            QJsonObject paths;

            for (const auto &r : routeList) {
                QJsonObject methodObj;
                methodObj["summary"] = r.description;

                // Query parameters
                if (!r.queryParams.isEmpty()) {
                    QJsonArray params;
                    for (const auto &qp : r.queryParams) {
                        QJsonObject p;
                        p["name"] = qp.name;
                        p["in"] = "query";
                        p["required"] = qp.required;
                        p["schema"] = QJsonObject{{"type", qp.type}};
                        params.append(p);
                    }
                    methodObj["parameters"] = params;
                }

                // Request body
                if (!r.requestSchema.isEmpty()) {
                    QJsonObject content;
                    QJsonObject appJson;
                    QJsonObject schema;
                    schema["$ref"] = "#/components/schemas/" + r.requestSchema;
                    appJson["schema"] = schema;
                    content["application/json"] = appJson;

                    methodObj["requestBody"] = QJsonObject{
                        {"required", true},
                        {"content", content}
                    };
                }

                // Responses
                QJsonObject responses;
                QJsonObject resp200;
                resp200["description"] = "Successful response";

                if (!r.responseSchema.isEmpty()) {
                    QJsonObject content;
                    QJsonObject appJson;
                    QJsonObject schema;
                    schema["$ref"] = "#/components/schemas/" + r.responseSchema;
                    appJson["schema"] = schema;
                    content["application/json"] = appJson;
                    resp200["content"] = content;
                }

                responses["200"] = resp200;
                methodObj["responses"] = responses;

                QJsonObject pathItem = paths.value(r.path).toObject();
                pathItem[r.method.toLower()] = methodObj;
                paths[r.path] = pathItem;
            }

            openapi["paths"] = paths;

            // Components
            QJsonObject components;
            QJsonObject schemas;
            for (const auto &s : schemaList) {
                schemas[s.name] = s.schema;
            }
            components["schemas"] = schemas;
            openapi["components"] = components;

            return QHttpServerResponse("application/json",
                                       QJsonDocument(openapi).toJson());
        }
        );

    registerRoute("GET", "/rest-ui", "REST UI frontend",
        [](const QHttpServerRequest &) {
            QFile file(":/rest-ui/index.html");
            if (!file.open(QIODevice::ReadOnly)) {
                return QHttpServerResponse("text/plain", "REST UI not found",
                                           QHttpServerResponse::StatusCode::InternalServerError);
            }
            QByteArray html = file.readAll();
            return QHttpServerResponse("text/html; charset=utf-8", html,
                                       QHttpServerResponse::StatusCode::Ok);
        }
        );
}

void ApiServer::setupSchemas(){
    //
    // ────────────────────────────────────────────────────────────────
    //  SCHEMAS
    // ────────────────────────────────────────────────────────────────
    //

    // Echo example schemas
    registerSchema("EchoRequest", QJsonObject{
                                      {"type", "object"},
                                      {"properties", QJsonObject{
                                                         {"message", QJsonObject{{"type", "string"}}}
                                                     }},
                                      {"required", QJsonArray{"message"}}
                                  });

    registerSchema("EchoResponse", QJsonObject{
                                       {"type", "object"},
                                       {"properties", QJsonObject{
                                                          {"received", QJsonObject{
                                                                           {"$ref", "#/components/schemas/EchoRequest"}
                                                                       }}
                                                      }}
                                   });

    // Player schema
    registerSchema("Player", QJsonObject{
                                 {"type", "object"},
                                 {"properties", QJsonObject{
                                                    {"id", QJsonObject{{"type", "integer"}}},
                                                    {"name", QJsonObject{{"type", "string"}}},
                                                    {"white", QJsonObject{{"type", "boolean"}}}
                                                }},
                                 {"required", QJsonArray{"id","name","white"}}
                             });

    // Move schema
    registerSchema("Move", QJsonObject{
                               {"type", "object"},
                               {"properties", QJsonObject{
                                                  {"fromX", QJsonObject{{"type", "integer"}}},
                                                  {"fromY", QJsonObject{{"type", "integer"}}},
                                                  {"toX", QJsonObject{{"type", "integer"}}},
                                                  {"toY", QJsonObject{{"type", "integer"}}},
                                                  {"resign", QJsonObject{{"type", "boolean"}}},
                                                  {"drawOffer", QJsonObject{{"type", "boolean"}}}
                                              }},
                               {"required", QJsonArray{"fromX","fromY","toX","toY"}}
                           });

    // Matchmaking response
    registerSchema("MatchmakingResponse", QJsonObject{
                                              {"type", "object"},
                                              {"properties", QJsonObject{
                                                                 {"status", QJsonObject{{"type", "string"}}},
                                                                 {"gameId", QJsonObject{{"type", "integer"}}},
                                                                 {"whitePlayerId", QJsonObject{{"type", "integer"}}},
                                                                 {"blackPlayerId", QJsonObject{{"type", "integer"}}}
                                                             }}
                                          });

    // Move request
    registerSchema("MoveRequest", QJsonObject{
                                      {"type", "object"},
                                      {"properties", QJsonObject{
                                                         {"gameId", QJsonObject{{"type", "integer"}}},
                                                         {"playerId", QJsonObject{{"type", "integer"}}},
                                                         {"fromX", QJsonObject{{"type", "integer"}}},
                                                         {"fromY", QJsonObject{{"type", "integer"}}},
                                                         {"toX", QJsonObject{{"type", "integer"}}},
                                                         {"toY", QJsonObject{{"type", "integer"}}},
                                                         {"resign", QJsonObject{{"type", "boolean"}}},
                                                         {"drawOffer", QJsonObject{{"type", "boolean"}}}
                                                     }},
                                      {"required", QJsonArray{"gameId","playerId","fromX","fromY","toX","toY"}}
                                  });

    // Move response
    registerSchema("MoveResponse", QJsonObject{
                                       {"type", "object"},
                                       {"properties", QJsonObject{
                                                          {"valid", QJsonObject{{"type", "boolean"}}},
                                                          {"error", QJsonObject{{"type", "string"}}},
                                                          {"status", QJsonObject{{"type", "integer"}}}
                                                      }}
                                   });

    // Player registration response
    registerSchema("RegisterPlayerResponse", QJsonObject{
                                                 {"type", "object"},
                                                 {"properties", QJsonObject{
                                                                    {"id", QJsonObject{{"type", "integer"}}},
                                                                    {"name", QJsonObject{{"type", "string"}}},
                                                                    {"white", QJsonObject{{"type", "boolean"}}}
                                                                }},
                                                 {"required", QJsonArray{"id","name","white"}}
                                             });

    // Game List Response
    registerSchema("GameListResponse", QJsonObject{
                                           {"type", "object"},
                                           {"properties", QJsonObject{
                                                              {"games", QJsonObject{
                                                                            {"type", "array"},
                                                                            {"items", QJsonObject{{"$ref", "#/components/schemas/GameSummary"}}}
                                                                        }}
                                                          }},
                                           {"required", QJsonArray{"games"}}
                                       });

    // Game Replay Response
    registerSchema("GameReplayResponse", QJsonObject{
                                             {"type", "object"},
                                             {"properties", QJsonObject{
                                                                {"gameId", QJsonObject{{"type", "integer"}}},
                                                                {"moves", QJsonObject{
                                                                              {"type", "array"},
                                                                              {"items", QJsonObject{{"$ref", "#/components/schemas/Move"}}}
                                                                          }}
                                                            }},
                                             {"required", QJsonArray{"gameId","moves"}}
                                         });

    // Game summary schema
    registerSchema("GameSummary", QJsonObject{
                                      {"type", "object"},
                                      {"properties", QJsonObject{
                                                         {"gameId", QJsonObject{{"type", "integer"}}},
                                                         {"status", QJsonObject{{"type", "integer"}}},
                                                         {"statusText", QJsonObject{{"type", "string"}}},
                                                         {"firstPlayerId", QJsonObject{{"type", "integer"}}},
                                                         {"secondPlayerId", QJsonObject{{"type", "integer"}}}
                                                     }},
                                      {"required", QJsonArray{"gameId","status","statusText","firstPlayerId","secondPlayerId"}}
                                  });

    // Register game response
    registerSchema("RegisterGameResponse", QJsonObject{
                                               {"type", "object"},
                                               {"properties", QJsonObject{
                                                                  {"gameId", QJsonObject{{"type", "integer"}}},
                                                                  {"firstPlayerId", QJsonObject{{"type", "integer"}}},
                                                                  {"secondPlayerId", QJsonObject{{"type", "integer"}}},
                                                                  {"status", QJsonObject{{"type", "integer"}}}
                                                              }},
                                               {"required", QJsonArray{"gameId","firstPlayerId","secondPlayerId","status"}}
                                           });
}

void ApiServer::getServerPort(QCoreApplication &app) {
    parser.addOptions({
                       {"port", "The port the server listens on.", "port"},
                       });
    parser.addHelpOption();
    parser.process(app);

    portArg = parser.value("port").isEmpty()
                  ? 8080
                  : parser.value("port").toUShort();
}

void ApiServer::setServer(QCoreApplication &app) {
    parser.addOptions({
                       {"host", "The host the server listens on.", "host"},
                       });
    parser.addHelpOption();
    parser.process(app);

    hostName = parser.value("host").isEmpty()
                   ? QHostAddress::Any
                   : QHostAddress(QString::fromStdString(parser.value("host").toStdString()));

    tcpServer.listen(hostName, portArg);
    server.bind(&tcpServer);
}
