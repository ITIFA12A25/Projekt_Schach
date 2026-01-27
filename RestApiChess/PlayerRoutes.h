#ifndef PLAYERROUTES_H
#define PLAYERROUTES_H

#pragma once
#include "ApiRoute.h"
#include "UserService.h"
#include "Player.h"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>

class PlayerRoutes {
public:
    QList<ApiRoute> registerRoutes();

private:
    UserService *userService = UserService::getInstance();
    QHttpServerResponse registerPlayer(const QHttpServerRequest &req);
};

#endif // PLAYERROUTES_H
