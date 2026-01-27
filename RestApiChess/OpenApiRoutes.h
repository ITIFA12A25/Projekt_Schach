#ifndef OPENAPIROUTES_H
#define OPENAPIROUTES_H

#pragma once
#include "ApiRoute.h"
#include "RouteRegistrar.h"
#include "SchemaRegistry.h"
#include <QFile>
#include <QJsonArray>

class OpenApiRoutes {
public:
    OpenApiRoutes(RouteRegistrar *router, SchemaRegistry *schemaReg);

    QList<ApiRoute> registerRoutes();

private:
    RouteRegistrar *router;
    SchemaRegistry *schemaReg;

    QHttpServerResponse getRouting(const QHttpServerRequest &req);
};

#endif // OPENAPIROUTES_H
