#ifndef GAMESTATUSUTILS_H
#define GAMESTATUSUTILS_H

#pragma once
#include "GameStatus.h"
#include <QString>

inline QString gameStatusToString(GameStatus status) {
    switch (status) {
    case GameStatus::InProgress: return "InProgress";
    case GameStatus::Draw: return "Draw";
    case GameStatus::Stalemate: return "Stalemate";
    case GameStatus::FirstPlayerWin: return "FirstPlayerWin";
    case GameStatus::SecondPlayerWin: return "SecondPlayerWin";
    }
    return "Unknown";
}

#endif // GAMESTATUSUTILS_H
