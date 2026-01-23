#include <QCoreApplication>
#include "apiserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ApiServer *server = ApiServer::getInstance();
    server->Start(app);

    return app.exec();
}
