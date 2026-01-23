#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QList>
#include <QFile>
#include <QJsonDocument>

#include "Player.h"
#include "Game.h"

class Persistence {
public:
    Persistence();
    static Persistence *getInstance(void);
    static bool savePlayers(const QList<Player*> &players, const QString &filePath);
    static bool loadPlayers(QList<Player*> &players, const QString &filePath);

    static bool saveGames(const QList<Game*> &games, const QString &filePath);
    static bool loadGames(QList<Game*> &games, const QList<Player*> &players, const QString &filePath);
private:
    static Persistence *instance;
};

#endif // PERSISTENCE_H
