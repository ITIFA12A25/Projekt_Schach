#ifndef MATCHMAKINGROUTES_H
#define MATCHMAKINGROUTES_H

#pragma once
#include "MatchmakingService.h"
#include "UserService.h"
#include "GameStatusUtils.h"
#include "ApiRoute.h"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>

class MatchmakingRoutes {
public:
    QList<ApiRoute> registerRoutes();
private:
    QHttpServerResponse matchPlayers(const QHttpServerRequest &req);
    MatchmakingService *matchmaking = MatchmakingService::getInstance();
    UserService *userService = UserService::getInstance();
};


#endif // MATCHMAKINGROUTES_H
