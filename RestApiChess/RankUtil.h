#ifndef RANKUTIL_H
#define RANKUTIL_H

#include <QString>
#include "Piece.h"

inline QString rankToString(Rank r) {
    switch (r) {
    case Rank::King:   return "king";
    case Rank::Queen:  return "queen";
    case Rank::Rook:   return "rook";
    case Rank::Bishop: return "bishop";
    case Rank::Knight: return "knight";
    case Rank::Pawn:   return "pawn";
    }
    return "unknown";
}


#endif // RANKUTIL_H
