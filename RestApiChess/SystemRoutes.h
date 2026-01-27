#ifndef SYSTEMROUTES_H
#define SYSTEMROUTES_H

#pragma once
#include "ApiRoute.h"
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>

class SystemRoutes {
public:
    QList<ApiRoute> registerRoutes();
};

#endif // SYSTEMROUTES_H
