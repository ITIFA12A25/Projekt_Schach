#ifndef APISERVER_H
#define APISERVER_H

using namespace std;

#pragma once
#include <QTcpServer>
#include <QHttpServer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QDebug>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QList>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QString>

#include "MatchmakingService.h"
#include "GameRepository.h"
#include "UserService.h"
#include "MoveValidator.h"
#include "ApiRoute.h"
#include "RouteRegistrar.h"
#include "SchemaRegistry.h"

#include "MatchmakingRoutes.h"
#include "GameRoutes.h"
#include "SystemRoutes.h"
#include "PlayerRoutes.h"
#include "OpenApiRoutes.h"

#define SCHEME "http"
#define HOST "127.0.0.1"
#define PORT 8080;

class ApiServer {
public:
    ApiServer();
    static ApiServer *getInstance(void);
    void Start(QCoreApplication &app);

    void registerRoute(
        const HttpMethod &method,
        const QString &path,
        const QString &description,
        std::function<QHttpServerResponse(const QHttpServerRequest&)> handler,
        const QString &requestSchema,
        const QString &responseSchema,
        const QList<ApiQueryParam> &queryParams
        );

private:
    static ApiServer *instance;
    void setupRoutes();
    void setupSchemas();

    void getServerPort(QCoreApplication &app);
    void setServer(QCoreApplication &app);

    QHttpServer server;
    QTcpServer tcpServer;
    QCommandLineParser parser;
    quint16 portArg = 0;
    QHostAddress hostName = QHostAddress::Any;

    MatchmakingService *matchmaking = MatchmakingService::getInstance();
    GameRepository *gameRepo = GameRepository::getInstance();
    UserService *userService = UserService::getInstance();
    RouteRegistrar *router = RouteRegistrar::getInstance(server);
    SchemaRegistry *schemaReg = SchemaRegistry::getInstance();
    MoveValidator moveValidator;


    GameRoutes *gameRoutes = new GameRoutes(&moveValidator);
    PlayerRoutes *playerRoutes = new PlayerRoutes();
    MatchmakingRoutes *mmRoutes = new MatchmakingRoutes();
    SystemRoutes *sysRoutes = new SystemRoutes();
    OpenApiRoutes *openApiRoutes = new OpenApiRoutes(router, schemaReg);
};
#endif // APISERVER_H
