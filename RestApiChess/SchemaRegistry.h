#ifndef SCHEMAREGISTRY_H
#define SCHEMAREGISTRY_H

#pragma once
#include "ApiRoute.h"

class SchemaRegistry {
public:
    static SchemaRegistry *getInstance(void);
    void registerSchema(const QString &name, const QJsonObject &schema);
    const QList<ApiSchema> &getSchemas() const { return schemas; }

private:
    static SchemaRegistry *instance;
    QList<ApiSchema> schemas;
};


#endif // SCHEMAREGISTRY_H
