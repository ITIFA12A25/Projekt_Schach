#ifndef ROUTEREGISTRAR_H
#define ROUTEREGISTRAR_H

#pragma once
#include "ApiRoute.h"
#include <QHttpServer>

class RouteRegistrar {
public:
    explicit RouteRegistrar(QHttpServer &server);
    static RouteRegistrar *getInstance(QHttpServer &server);
    void addRoute(const ApiRoute &route);
    void registerAll();

    const QList<ApiRoute> &getRoutes() const { return routes; }

private:
    static RouteRegistrar *instance;
    QHttpServer &server;
    QList<ApiRoute> routes;
};


#endif // ROUTEREGISTRAR_H
