#include "RouteRegistrar.h"

RouteRegistrar::RouteRegistrar(QHttpServer &server)
    : server(server) {}

RouteRegistrar* RouteRegistrar::instance = nullptr;

RouteRegistrar* RouteRegistrar::getInstance(QHttpServer &server)
{
    if ( instance == nullptr )
        instance = new RouteRegistrar(server);
    return instance;
}

void RouteRegistrar::addRoute(const ApiRoute &route) {
    routes.append(route);
}

static void bindRoute(QHttpServer &server, const ApiRoute &r) {
    auto wrapper = [handler = r.handler](const QHttpServerRequest &req) {
        return handler(req);
    };

    switch (r.method) {
    case HttpMethod::Get:
        server.route(r.path, wrapper);
        break;
    case HttpMethod::Post:
        server.route(r.path, QHttpServerRequest::Method::Post, wrapper);
        break;
    case HttpMethod::Put:
        server.route(r.path, QHttpServerRequest::Method::Put, wrapper);
        break;
    case HttpMethod::Delete:
        server.route(r.path, QHttpServerRequest::Method::Delete, wrapper);
        break;
    }
}

void RouteRegistrar::registerAll() {
    for (const auto &r : routes) {
        bindRoute(server, r);
    }
}

