#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "IPlayer.h"

class HumanPlayer : public IPlayer {
public:
    HumanPlayer(int id, const QString &name, bool white)
        : id(id), name(name), white(white) {}

    bool isWhite() const override { return white; }
    int getId() const override { return id; }
    QString getName() const override { return name; }
    void setName(QString &playerName) override { name = playerName; }

private:
    int id;
    QString name;
    bool white;
};

#endif // HUMANPLAYER_H
