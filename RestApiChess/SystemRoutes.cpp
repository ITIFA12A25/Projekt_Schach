#include "SystemRoutes.h"

QList<ApiRoute> SystemRoutes::registerRoutes() {
    QList<ApiRoute> routes;

    routes.append(ApiRoute{
        HttpMethod::Get,
        "/",
        "Root endpoint",
        [](const QHttpServerRequest &) {
            return QHttpServerResponse("text/plain", "Hello from Qt REST API!");
        },
        {},
        {},
        {}
    });

    routes.append(ApiRoute{
        HttpMethod::Get,
        "/hello",
        "Simple hello endpoint",
        [](const QHttpServerRequest &) {
            return QHttpServerResponse("text/plain", "Hello from Qt REST API!");
        },
        {},
        {},
        {}
    });

    routes.append(ApiRoute{
        HttpMethod::Get,
        "/sum",
        "Calculate sum of a and b",
        [](const QHttpServerRequest &req) {
            QUrlQuery q(req.url().query());
            bool okA, okB;
            int a = q.queryItemValue("a").toInt(&okA);
            int b = q.queryItemValue("b").toInt(&okB);

            QJsonObject json;
            if (!okA || !okB) {
                json["error"] = "Missing or invalid parameters";
                return QHttpServerResponse("application/json",
                                           QJsonDocument(json).toJson(),
                                           QHttpServerResponse::StatusCode::BadRequest);
            }

            json["a"] = a;
            json["b"] = b;
            json["sum"] = a + b;

            return QHttpServerResponse("application/json",
                                       QJsonDocument(json).toJson());
        },
        "",
        "",
        { {"a","integer",true}, {"b","integer",true} }
    });

    return routes;
}
