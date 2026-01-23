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
#include "HumanPlayer.h"

#define SCHEME "http"
#define HOST "127.0.0.1"
#define PORT 8080;

struct ApiQueryParam {
    QString name;
    QString type; // e.g. "string", "integer"
    bool required;
};

struct ApiRoute {
    QString method;
    QString path;
    QString description;
    QString requestSchema; // name in components/schemas
    QString responseSchema; // name in components/schemas
    QList<ApiQueryParam> queryParams;
};

struct ApiSchema {
    QString name;
    QJsonObject schema; // OpenAPI schema object
};

class ApiServer {
public:
    ApiServer();
    static ApiServer *getInstance(void);
    void Start(QCoreApplication &app);
private:
    static ApiServer *instance;
    void setupRoutes();
    void setupSchemas();
    void registerRoute( const QString &method,
                       const QString &path,
                       const QString &description,
                       function<QHttpServerResponse(const QHttpServerRequest&)> handler,
                       const QString &requestSchema = {},
                       const QString &responseSchema = {},
                       const QList<ApiQueryParam> &queryParams = {}
                       );
    void registerSchema(const QString &name, const QJsonObject &schema);
    void getServerPort(QCoreApplication &app);
    void setServer();

    QHttpServer server;
    QTcpServer tcpServer;
    QCommandLineParser parser;
    quint16 portArg = 0;

    QList<ApiRoute> routeList;
    QList<ApiSchema> schemaList;

    MatchmakingService *matchmaking = MatchmakingService::getInstance();
    GameRepository *gameRepo = GameRepository::getInstance();
    UserService *userService = UserService::getInstance();
    MoveValidator moveValidator;
    // For demo: simple in-memory players
    QMap<int, HumanPlayer*> players;
};
#endif // APISERVER_H
