#include "Persistence.h"

Persistence* Persistence::instance = nullptr;

Persistence::Persistence(){

}

Persistence* Persistence::getInstance()
{
    if ( instance == nullptr )
        instance = new Persistence;
    return instance;
}

bool Persistence::savePlayers(const QVector<Player*> &players, const QString &filePath) {
    QJsonArray arr;
    for (auto p : players) {
        QJsonObject o;
        o["id"] = p->getId();
        o["name"] = p->getName();
        o["white"] = p->isWhite();
        arr.append(o);
    }
    QJsonDocument doc(arr);
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(doc.toJson());
    return true;
}

bool Persistence::loadPlayers(QVector<Player*> &players, const QString &filePath) {
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return false;
    QJsonArray arr = doc.array();
    for (auto v : arr) {
        QJsonObject o = v.toObject();
        int id = o["id"].toInt();
        QString name = o["name"].toString();
        bool white = o["white"].toBool();
        players.append(new Player(id, name, white));
    }
    return true;
}

bool Persistence::saveGames(const QVector<Game*> &games, const QString &filePath) {
    QJsonArray arr;
    for (auto g : games) {
        QJsonObject o;
        o["id"] = g->id();
        o["firstPlayerId"] = g->firstPlayer()->getId();
        o["secondPlayerId"] = g->secondPlayer()->getId();
        o["status"] = (int)g->status();

        QJsonArray movesArr;
        for (const auto &m : g->moves()) {
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

    QJsonDocument doc(arr);
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(doc.toJson());
    return true;
}

bool Persistence::loadGames(QVector<Game*> &games, const QVector<Player*> &players, const QString &filePath) {
    auto findPlayer = [&](int id) -> Player* {
        for (auto p : players) if (p->getId() == id) return p;
        return nullptr;
    };

    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return false;
    QJsonArray arr = doc.array();

    for (auto v : arr) {
        QJsonObject o = v.toObject();
        int id = o["id"].toInt();
        int p1Id = o["firstPlayerId"].toInt();
        int p2Id = o["secondPlayerId"].toInt();
        auto p1 = findPlayer(p1Id);
        auto p2 = findPlayer(p2Id);
        if (!p1 || !p2) continue;

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
            QString err;
            g->applyMove(m, err); // rebuild board
        }

        games.append(g);
    }
    return true;
}
