#include "apiserver.h"
#include <QFile>
#include <QDebug>

ApiServer::ApiServer() {
    // Load players
    QVector<HumanPlayer*> loadedPlayers;
    if (!Persistence::loadPlayers(loadedPlayers, "players.json")) {
        // fallback: create some defaults
        loadedPlayers.append(new HumanPlayer(1, "Alice", true));
        loadedPlayers.append(new HumanPlayer(2, "Bob", false));
    }
    for (auto p : loadedPlayers) {
        players.insert(p->getId(), p);
    }
    // Load games
    QVector<HumanPlayer*> playerVec = loadedPlayers;
    gameRepo->loadAll("games.json", playerVec);
}

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

    setServer();
    qDebug() << "HTTP server started.";

    qDebug() << "Top-level resources:" << QDir(":/").entryList();
    qDebug() << "Inside /rest-ui:" << QDir(":/rest-ui").entryList();
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
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(req.body(), &err);
            if (err.error != QJsonParseError::NoError || !doc.isObject()) {
                return QHttpServerResponse("application/json",
                                           R"({"error":"Invalid JSON"})",
                                           QHttpServerResponse::StatusCode::BadRequest);
            }

            int playerId = doc["playerId"].toInt();
            auto player = players.value(playerId, nullptr);
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
            json["status"] = "matched";
            json["gameId"] = game->id();
            json["whitePlayerId"] = game->firstPlayer()->getId();
            json["blackPlayerId"] = game->secondPlayer()->getId();

            return QHttpServerResponse("application/json",
                                       QJsonDocument(json).toJson());
        },
        "MatchmakingRequest",
        "MatchmakingResponse"
        );

    //
    // ────────────────────────────────────────────────────────────────
    //  GAME LISTING
    // ────────────────────────────────────────────────────────────────
    //

    registerRoute("GET", "/api/games", "List games for a player",
        [this](const QHttpServerRequest &req) {
            QUrlQuery q(req.url().query());
            int playerId = q.queryItemValue("playerId").toInt();

            auto games = gameRepo->gamesForPlayer(playerId);
            QJsonArray arr;

            for (auto g : games) {
                QJsonObject o;
                o["gameId"] = g->id();
                o["status"] = (int)g->status();
                o["firstPlayerId"] = g->firstPlayer()->getId();
                o["secondPlayerId"] = g->secondPlayer()->getId();
                arr.append(o);
            }

            QJsonObject root;
            root["games"] = arr;

            return QHttpServerResponse("application/json",
                                       QJsonDocument(root).toJson());
        },
        {},
        "GameSummary"
        );

    //
    // ────────────────────────────────────────────────────────────────
    //  GAME REPLAY
    // ────────────────────────────────────────────────────────────────
    //

    registerRoute("GET", "/api/games/replay", "Get replay moves for a game",
        [this](const QHttpServerRequest &req) {
            QUrlQuery q(req.url().query());
            int gameId = q.queryItemValue("gameId").toInt();

            const Game *game = gameRepo->getGame(gameId);
            if (!game) {
                return QHttpServerResponse("application/json",
                                           R"({"error":"Game not found"})",
                                           QHttpServerResponse::StatusCode::NotFound);
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
        }
        );

    //
    // ────────────────────────────────────────────────────────────────
    //  MOVE SUBMISSION
    // ────────────────────────────────────────────────────────────────
    //

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

            Game *game = matchmaking->getGame(gameId);
            if (!game) {
                return QHttpServerResponse("application/json",
                                           R"({"error":"Game not found"})",
                                           QHttpServerResponse::StatusCode::NotFound);
            }

            Move move;
            move.from = { o["fromX"].toInt(), o["fromY"].toInt() };
            move.to   = { o["toX"].toInt(),   o["toY"].toInt() };
            move.resign = o["resign"].toBool(false);
            move.drawOffer = o["drawOffer"].toBool(false);

            QString error;
            bool ok = moveValidator.validateAndApply(*game, move, error);

            QJsonObject resp;
            resp["valid"] = ok;
            if (!ok) resp["error"] = error;
            resp["status"] = (int)game->status();

            return QHttpServerResponse("application/json",
                                       QJsonDocument(resp).toJson());
        },
        "MoveRequest",
        "MoveResponse"
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

    // Game summary schema
    registerSchema("GameSummary", QJsonObject{
                                      {"type", "object"},
                                      {"properties", QJsonObject{
                                                         {"gameId", QJsonObject{{"type", "integer"}}},
                                                         {"status", QJsonObject{{"type", "integer"}}},
                                                         {"firstPlayerId", QJsonObject{{"type", "integer"}}},
                                                         {"secondPlayerId", QJsonObject{{"type", "integer"}}}
                                                     }},
                                      {"required", QJsonArray{"gameId","status","firstPlayerId","secondPlayerId"}}
                                  });

    // Matchmaking request
    registerSchema("MatchmakingRequest", QJsonObject{
                                             {"type", "object"},
                                             {"properties", QJsonObject{
                                                                {"playerId", QJsonObject{{"type", "integer"}}}
                                                            }},
                                             {"required", QJsonArray{"playerId"}}
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

void ApiServer::setServer() {
    tcpServer.listen(QHostAddress::Any, portArg);
    server.bind(&tcpServer);
}
