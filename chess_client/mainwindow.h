#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include "chessapiclient.h"
#include "chessstate.h"
#include "chessboardwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // UI button slots
    void onRegisterPlayerClicked();
    void onMatchmakingClicked();
    void onGetGamesClicked();
    void onRegisterGameClicked();
    void onGetBoardClicked();
    void onGetReplayClicked();
    void onSendMoveClicked();

    // Board interaction
    void onSquareClicked(int x, int y);

    // API response slots
    void updatePlayer(const QJsonObject &json);
    void updateMatchmaking(const QJsonObject &json);
    void updateGames(const QJsonObject &json);
    void updateGameRegistered(const QJsonObject &json);
    void updateBoard(const QJsonObject &json);
    void updateReplay(const QJsonObject &json);
    void updateMove(const QJsonObject &json);
    void onError(const QString &op, const QString &message, const QJsonObject &json);

private:
    Ui::MainWindow *ui;
    ChessApiClient api;
    ChessState state;
    ChessBoardWidget *boardWidget;

    void showJson(const QJsonObject &json);
    void refreshBoard();
};

#endif // MAINWINDOW_H
