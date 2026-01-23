#ifndef IPLAYER_H
#define IPLAYER_H
using namespace std;

#include <QString>

class IPlayer {
public:
    virtual ~IPlayer() = default;
    virtual bool isWhite() const = 0;
    virtual int getId() const = 0;
    virtual QString getName() const = 0;
    virtual void setName(QString &playerName);
};

#endif // IPLAYER_H
