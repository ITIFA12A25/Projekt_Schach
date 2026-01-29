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
    qDebug() << "Schemas registered:" << schemaReg->getSchemas().size();

    setupRoutes();
    qDebug() << "Routes registered:" << router->getRoutes().size();

    // Bind all routes to
    router->registerAll();

    // Start server
    setServer(app);
    qDebug() << "HTTP server started.";

    qDebug().noquote() << "REST API Endpoint Docs:\t" << "http://" + hostName.toString() + ":" + QString::number(portArg) + "/rest-ui";
    // On shutdown call:
    // gameRepo->saveAll("games.json");
    // Persistence::savePlayers(playerVec, "players.json");
}

void ApiServer::setupRoutes() {
    for (ApiRoute route : gameRoutes->registerRoutes()){
        router->addRoute(route);
    }
    for (ApiRoute route : playerRoutes->registerRoutes()){
        router->addRoute(route);
    }
    for (ApiRoute route : mmRoutes->registerRoutes()){
            router->addRoute(route);
    }
    for (ApiRoute route : sysRoutes->registerRoutes()){
        router->addRoute(route);
    }
    for (ApiRoute route : openApiRoutes->registerRoutes()){
        router->addRoute(route);
    }
}

void ApiServer::setupSchemas(){
    //
    // ────────────────────────────────────────────────────────────────
    //  SCHEMAS
    // ────────────────────────────────────────────────────────────────
    //

    // Echo example schemas
    schemaReg->registerSchema("EchoRequest", QJsonObject{
                                      {"type", "object"},
                                      {"properties", QJsonObject{
                                                         {"message", QJsonObject{{"type", "string"}}}
                                                     }},
                                      {"required", QJsonArray{"message"}}
                                  });

    schemaReg->registerSchema("EchoResponse", QJsonObject{
                                       {"type", "object"},
                                       {"properties", QJsonObject{
                                                          {"received", QJsonObject{
                                                                           {"$ref", "#/components/schemas/EchoRequest"}
                                                                       }}
                                                      }}
                                   });

    // Player schema
    schemaReg->registerSchema("Player", QJsonObject{
                                 {"type", "object"},
                                 {"properties", QJsonObject{
                                                    {"id", QJsonObject{{"type", "integer"}}},
                                                    {"name", QJsonObject{{"type", "string"}}},
                                                    {"white", QJsonObject{{"type", "boolean"}}}
                                                }},
                                 {"required", QJsonArray{"id","name","white"}}
                             });

    // Move schema
    schemaReg->registerSchema("Move", QJsonObject{
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
    schemaReg->registerSchema("MatchmakingResponse", QJsonObject{
                                              {"type", "object"},
                                              {"properties", QJsonObject{
                                                                 {"status", QJsonObject{{"type", "string"}}},
                                                                 {"gameId", QJsonObject{{"type", "integer"}}},
                                                                 {"firstPlayerId", QJsonObject{{"type", "integer"}}},
                                                                 {"secondPlayerId", QJsonObject{{"type", "integer"}}}
                                                             }}
                                          });

    // Move request
    schemaReg->registerSchema("MoveRequest", QJsonObject{
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
    schemaReg->registerSchema("MoveResponse", QJsonObject{
                                       {"type", "object"},
                                       {"properties", QJsonObject{
                                                          {"valid", QJsonObject{{"type", "boolean"}}},
                                                          {"error", QJsonObject{{"type", "string"}}},
                                                          {"status", QJsonObject{{"type", "integer"}}}
                                                      }}
                                   });

    // Player registration response
    schemaReg->registerSchema("RegisterPlayerResponse", QJsonObject{
                                                 {"type", "object"},
                                                 {"properties", QJsonObject{
                                                                    {"id", QJsonObject{{"type", "integer"}}},
                                                                    {"name", QJsonObject{{"type", "string"}}},
                                                                    {"white", QJsonObject{{"type", "boolean"}}}
                                                                }},
                                                 {"required", QJsonArray{"id","name","white"}}
                                             });

    // Game List Response
    schemaReg->registerSchema("GameListResponse", QJsonObject{
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
    schemaReg->registerSchema("GameReplayResponse", QJsonObject{
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
    schemaReg->registerSchema("GameSummary", QJsonObject{
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
    schemaReg->registerSchema("RegisterGameResponse", QJsonObject{
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
