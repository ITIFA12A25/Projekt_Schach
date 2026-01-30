#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QList>
#include <QString>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "Player.h"
#include "Game.h"

class Persistence {
public:
    Persistence();
    static Persistence *getInstance(void);

    // Player persistence
    static bool savePlayers(const QList<Player*> &players, const QString &filePath);
    static bool loadPlayers(QList<Player*> &players, const QString &filePath);

    // Game persistence
    static bool saveGames(const QList<Game*> &games, const QString &filePath);
    static bool loadGames(QList<Game*> &games,
                          const QList<Player*> &players,
                          const QString &filePath);

private:
    static Persistence *instance;
    static Player* findPlayer(int id, QList<Player*>players);
};

#endif // PERSISTENCE_H
