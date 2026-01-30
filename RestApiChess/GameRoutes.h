#ifndef GAMEROUTES_H
#define GAMEROUTES_H

#pragma once
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "ApiRoute.h"
#include "GameRepository.h"
#include "GameStatusUtils.h"
#include "UserService.h"
#include "MoveValidator.h"
#include "RankUtil.h"

class GameRoutes {
public:
    GameRoutes(MoveValidator *validator);

    QList<ApiRoute> registerRoutes();

private:
    GameRepository *gameRepo = GameRepository::getInstance();
    UserService *userService = UserService::getInstance();
    MoveValidator *validator;

    QHttpServerResponse listGames(const QHttpServerRequest &req);
    QHttpServerResponse replay(const QHttpServerRequest &req);
    QHttpServerResponse registerGame(const QHttpServerRequest &req);
    QHttpServerResponse move(const QHttpServerRequest &req);
    QHttpServerResponse boardState(const QHttpServerRequest &req);
};


#endif // GAMEROUTES_H
