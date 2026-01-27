#include "OpenApiRoutes.h"

OpenApiRoutes::OpenApiRoutes(RouteRegistrar *router, SchemaRegistry *schemaReg)
    : router(router), schemaReg(schemaReg) {}

QList<ApiRoute> OpenApiRoutes::registerRoutes() {
    QList<ApiRoute> routes;

    routes.append(ApiRoute{
        HttpMethod::Get,
        "/openapi.json",
        "OpenAPI 3.0 specification",
        [this](const QHttpServerRequest &req) { return getRouting(req); },
        {},
        {},
        {}
    });

    routes.append(ApiRoute{
        HttpMethod::Get,
        "/rest-ui",
        "REST UI frontend",
        [](const QHttpServerRequest &) {
            QFile file(":/rest-ui/index.html");
            if (!file.open(QIODevice::ReadOnly)) {
                return QHttpServerResponse("text/plain",
                                           "REST UI not found",
                                           QHttpServerResponse::StatusCode::InternalServerError);
            }
            QByteArray html = file.readAll();
            return QHttpServerResponse("text/html; charset=utf-8",
                                       html,
                                       QHttpServerResponse::StatusCode::Ok);
        },
        {},
        {},
        {}
    });

    return routes;
}

QHttpServerResponse OpenApiRoutes::getRouting(const QHttpServerRequest &) {
    QJsonObject openapi;
    openapi["openapi"] = "3.0.0";

    QJsonObject info;
    info["title"] = "Qt Chess API";
    info["version"] = "1.0.0";
    openapi["info"] = info;

    QJsonObject paths;

    for (const auto &r : router->getRoutes()) {
        QJsonObject methodObj;
        methodObj["summary"] = r.description;

        // Query parameters
        if (!r.queryParams.isEmpty()) {
            QJsonArray params;
            for (const auto &qp : r.queryParams) {
                QJsonObject p;
                p["name"] = qp.name;
                p["in"] = "query";
                p["required"] = qp.required;
                p["schema"] = QJsonObject{{"type", qp.type}};
                params.append(p);
            }
            methodObj["parameters"] = params;
        }

        // Request body
        if (!r.requestSchema.isEmpty()) {
            QJsonObject content;
            QJsonObject appJson;
            QJsonObject schema;
            schema["$ref"] = "#/components/schemas/" + r.requestSchema;
            appJson["schema"] = schema;
            content["application/json"] = appJson;

            methodObj["requestBody"] = QJsonObject{
                {"required", true},
                {"content", content}
            };
        }

        // Responses
        QJsonObject responses;
        QJsonObject resp200;
        resp200["description"] = "Successful response";

        if (!r.responseSchema.isEmpty()) {
            QJsonObject content;
            QJsonObject appJson;
            QJsonObject schema;
            schema["$ref"] = "#/components/schemas/" + r.responseSchema;
            appJson["schema"] = schema;
            content["application/json"] = appJson;
            resp200["content"] = content;
        }

        responses["200"] = resp200;
        methodObj["responses"] = responses;

        QJsonObject pathItem = paths.value(r.path).toObject();

        QString methodName =
            (r.method == HttpMethod::Get)    ? "get" :
                (r.method == HttpMethod::Post)   ? "post" :
                (r.method == HttpMethod::Put)    ? "put" :
                "delete";

        pathItem[methodName] = methodObj;
        paths[r.path] = pathItem;
    }

    openapi["paths"] = paths;

    // Components
    QJsonObject components;
    QJsonObject schemas;
    for (const auto &s : schemaReg->getSchemas()) {
        schemas[s.name] = s.schema;
    }
    components["schemas"] = schemas;
    openapi["components"] = components;

    return QHttpServerResponse("application/json",
                               QJsonDocument(openapi).toJson());
}
