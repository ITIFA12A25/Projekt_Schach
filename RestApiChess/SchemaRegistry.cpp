#include "SchemaRegistry.h"

SchemaRegistry* SchemaRegistry::instance = nullptr;

SchemaRegistry* SchemaRegistry::getInstance()
{
    if ( instance == nullptr )
        instance = new SchemaRegistry;
    return instance;
}

void SchemaRegistry::registerSchema(const QString &name, const QJsonObject &schema) {
    schemas.append(ApiSchema{name, schema});
}

