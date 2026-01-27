#include "Persistence.h"

Persistence* Persistence::instance = nullptr;

Persistence::Persistence() {}

Persistence* Persistence::getInstance()
{
    if (!instance)
        instance = new Persistence;
    return instance;
}

bool Persistence::savePlayers(const QList<Player*> &players, const QString &filePath)
{
    QJsonArray arr;
    for (auto p : players) {
        QJsonObject o;
        o["id"] = p->getId();
        o["name"] = p->getName();
        o["white"] = p->isWhite();
        arr.append(o);
    }

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    f.write(QJsonDocument(arr).toJson());
    return true;
}

bool Persistence::loadPlayers(QList<Player*> &players, const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray())
        return false;

    for (auto v : doc.array()) {
        QJsonObject o = v.toObject();
        players.append(new Player(
            o["id"].toInt(),
            o["name"].toString(),
            o["white"].toBool()
            ));
    }
    return true;
}

bool Persistence::saveGames(const QList<Game*> &games, const QString &filePath)
{
    QJsonArray arr;

    for (auto g : games) {
        QJsonObject o;
        o["id"] = g->id();
        o["firstPlayerId"] = g->firstPlayer()->getId();
        o["secondPlayerId"] = g->secondPlayer()->getId();
        o["status"] = (int)g->status();

        QJsonArray movesArr;
        for (const Move &m : g->moves()) {
            QJsonObject mo;
            mo["fromX"] = m.from.x;
            mo["fromY"] = m.from.y;
            mo["toX"] = m.to.x;
            mo["toY"] = m.to.y;
            mo["resign"] = m.resign;
            mo["drawOffer"] = m.drawOffer;
            movesArr.append(mo);
        }
        o["moves"] = movesArr;

        arr.append(o);
    }

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly))
        return false;

    f.write(QJsonDocument(arr).toJson());
    return true;
}

bool Persistence::loadGames(QList<Game*> &games, const QList<Player*> &players, const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray())
        return false;

    for (auto v : doc.array()) {
        QJsonObject o = v.toObject();

        int id = o["id"].toInt();
        Player *p1 = findPlayer(o["firstPlayerId"].toVariant().toInt(), players);
        Player *p2 = findPlayer(o["secondPlayerId"].toVariant().toInt(), players);

        if (!p1 || !p2)
            continue;

        Game *g = new Game(id, p1, p2);
        g->setResult((GameStatus)o["status"].toInt());

        QJsonArray movesArr = o["moves"].toArray();
        for (auto mv : movesArr) {
            QJsonObject mo = mv.toObject();
            Move m;
            m.from = { mo["fromX"].toInt(), mo["fromY"].toInt() };
            m.to   = { mo["toX"].toInt(),   mo["toY"].toInt() };
            m.resign = mo["resign"].toBool();
            m.drawOffer = mo["drawOffer"].toBool();

            // still need applyMoveUnsafe() or similar
            QString err;
            g->applyMove(m, err);
        }

        games.append(g);
    }

    return true;
}

Player* Persistence::findPlayer(int id, QList<Player*>players) {
    for (auto p : players) {
        if (p->getId() == id)
            return p;
    }
    return nullptr;
}
