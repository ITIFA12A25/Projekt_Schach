#ifndef APIROUTE_H
#define APIROUTE_H

#pragma once
#include <QList>
#include <QJsonObject>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <functional>

struct ApiQueryParam {
    QString name;
    QString type;
    bool required;
};

enum class HttpMethod {
    Get,
    Post,
    Put,
    Delete
};

using RouteHandler = std::function<QHttpServerResponse(const QHttpServerRequest &)>;

struct ApiRoute {
    HttpMethod method;
    QString path;
    QString description;
    RouteHandler handler;
    QString requestSchema;
    QString responseSchema;
    QList<ApiQueryParam> queryParams;
};

struct ApiSchema {
    QString name;
    QJsonObject schema;
};


#endif // APIROUTE_H
