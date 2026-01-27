#include "PlayerRoutes.h"

QList<ApiRoute> PlayerRoutes::registerRoutes() {
    QList<ApiRoute> routes;

    routes.append(ApiRoute{
        HttpMethod::Post,
        "/api/players/register",
        "Register a new player",
        [this](const QHttpServerRequest &req) { return registerPlayer(req); },
        "",
        "RegisterPlayerResponse",
        { {"name","string",true} }
    });

    return routes;
}

QHttpServerResponse PlayerRoutes::registerPlayer(const QHttpServerRequest &req) {
    QUrlQuery q(req.url().query());
    QString name = q.queryItemValue("name");

    if (name.isEmpty()) {
        return QHttpServerResponse("application/json",
                                   R"({"error":"Missing 'name'"})",
                                   QHttpServerResponse::StatusCode::BadRequest);
    }

    userService->registerPlayer(&name);
    Player *created = userService->getPlayer(name);

    QJsonObject resp;
    resp["id"] = created->getId();
    resp["name"] = created->getName();
    resp["white"] = created->isWhite();

    return QHttpServerResponse("application/json",
                               QJsonDocument(resp).toJson());
}
