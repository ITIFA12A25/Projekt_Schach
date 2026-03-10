#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create board widget inside container
    boardWidget = new ChessBoardWidget(this);
    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(boardWidget);
    ui->chessBoardContainer->setLayout(layout);

    // Connect UI buttons
    connect(ui->registerPlayerButton, &QPushButton::clicked,
            this, &MainWindow::onRegisterPlayerClicked);
    connect(ui->matchmakingButton, &QPushButton::clicked,
            this, &MainWindow::onMatchmakingClicked);
    connect(ui->getGamesButton, &QPushButton::clicked,
            this, &MainWindow::onGetGamesClicked);
    connect(ui->registerGameButton, &QPushButton::clicked,
            this, &MainWindow::onRegisterGameClicked);
    connect(ui->getBoardButton, &QPushButton::clicked,
            this, &MainWindow::onGetBoardClicked);
    connect(ui->getReplayButton, &QPushButton::clicked,
            this, &MainWindow::onGetReplayClicked);
    connect(ui->sendMoveButton, &QPushButton::clicked,
            this, &MainWindow::onSendMoveClicked);

    // Board clicks
    connect(boardWidget, &ChessBoardWidget::squareClicked,
            this, &MainWindow::onSquareClicked);

    // API responses
    connect(&api, &ChessApiClient::playerRegistered,
            this, &MainWindow::updatePlayer);
    connect(&api, &ChessApiClient::matchmakingResult,
            this, &MainWindow::updateMatchmaking);
    connect(&api, &ChessApiClient::gamesReceived,
            this, &MainWindow::updateGames);
    connect(&api, &ChessApiClient::gameRegistered,
            this, &MainWindow::updateGameRegistered);
    connect(&api, &ChessApiClient::boardReceived,
            this, &MainWindow::updateBoard);
    connect(&api, &ChessApiClient::replayReceived,
            this, &MainWindow::updateReplay);
    connect(&api, &ChessApiClient::moveResponse,
            this, &MainWindow::updateMove);
    connect(&api, &ChessApiClient::errorReceived,
            this, &MainWindow::onError);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// -------- UI → API --------

void MainWindow::onRegisterPlayerClicked()
{
    QString name = ui->playerNameEdit->text().trimmed();
    if (name.isEmpty()) {
        ui->responseBox->setText("Enter a name.");
        return;
    }
    api.registerPlayer(name);
}

void MainWindow::onMatchmakingClicked()
{
    int id = ui->playerIdEdit->text().toInt();
    api.matchmaking(id);
}

void MainWindow::onGetGamesClicked()
{
    int id = ui->playerIdEdit->text().toInt();
    api.getGames(id);
}

void MainWindow::onRegisterGameClicked()
{
    int p1 = ui->firstPlayerIdEdit->text().toInt();
    int p2 = ui->secondPlayerIdEdit->text().toInt();
    api.registerGame(p1, p2);
}

void MainWindow::onGetBoardClicked()
{
    int gameId = ui->gameIdEdit->text().toInt();
    api.getBoard(gameId);
}

void MainWindow::onGetReplayClicked()
{
    int gameId = ui->gameIdEdit->text().toInt();
    api.getReplay(gameId);
}

void MainWindow::onSendMoveClicked()
{
    if (!state.hasSelection) {
        ui->responseBox->setText("Select a piece on the board first.");
        return;
    }

    QJsonObject move;
    move["gameId"]   = ui->gameIdEdit->text().toInt();
    move["playerId"] = ui->playerIdEdit->text().toInt();
    move["fromX"]    = state.selX;
    move["fromY"]    = state.selY;
    move["toX"]      = ui->toXEdit->text().toInt();
    move["toY"]      = ui->toYEdit->text().toInt();
    move["drawOffer"]= false;
    move["resign"]   = false;

    api.sendMove(move);
}

// -------- Board interaction --------

void MainWindow::onSquareClicked(int x, int y)
{
    // First click: select source
    if (!state.hasSelection) {
        state.hasSelection = true;
        state.selX = x;
        state.selY = y;
        boardWidget->setSelection(x, y, true);

        ui->fromXEdit->setText(QString::number(x));
        ui->fromYEdit->setText(QString::number(y));
        ui->responseBox->setText(QString("Selected from (%1,%2)").arg(x).arg(y));
        return;
    }

    // Second click: destination
    boardWidget->setSelection(state.selX, state.selY, false);

    int fromX = state.selX;
    int fromY = state.selY;
    int toX = x;
    int toY = y;

    state.hasSelection = false;
    state.selX = state.selY = -1;

    ui->fromXEdit->setText(QString::number(fromX));
    ui->fromYEdit->setText(QString::number(fromY));
    ui->toXEdit->setText(QString::number(toX));
    ui->toYEdit->setText(QString::number(toY));

    QJsonObject move;
    move["gameId"]   = ui->gameIdEdit->text().toInt();
    move["playerId"] = ui->playerIdEdit->text().toInt();
    move["fromX"]    = fromX;
    move["fromY"]    = fromY;
    move["toX"]      = toX;
    move["toY"]      = toY;
    move["drawOffer"]= false;
    move["resign"]   = false;

    api.sendMove(move);
}

// -------- API → UI --------

void MainWindow::showJson(const QJsonObject &json)
{
    ui->responseBox->setText(QJsonDocument(json).toJson(QJsonDocument::Indented));
}

void MainWindow::refreshBoard()
{
    int gameId = ui->gameIdEdit->text().toInt();
    if (gameId > 0)
        api.getBoard(gameId);
}

void MainWindow::updatePlayer(const QJsonObject &json)
{
    state.registeredPlayer = json;
    showJson(json);
}

void MainWindow::updateMatchmaking(const QJsonObject &json)
{
    state.matchmakingInfo = json;
    showJson(json);
}

void MainWindow::updateGames(const QJsonObject &json)
{
    state.games = json["games"].toArray();
    showJson(json);
}

void MainWindow::updateGameRegistered(const QJsonObject &json)
{
    showJson(json);
}

void MainWindow::updateBoard(const QJsonObject &json)
{
    state.board = json;
    showJson(json);
    boardWidget->updateBoard(json);
}

void MainWindow::updateReplay(const QJsonObject &json)
{
    state.replayMoves = json["moves"].toArray();
    showJson(json);
}

void MainWindow::updateMove(const QJsonObject &json)
{
    state.lastMoveResponse = json;
    showJson(json);

    // If move invalid or error, still refresh board to show current state
    refreshBoard();
}

void MainWindow::onError(const QString &op, const QString &message, const QJsonObject &json)
{
    QJsonObject out = json;
    out["clientErrorOp"] = op;
    out["clientErrorMessage"] = message;
    showJson(out);

    // On any error, refresh board to show actual state
    refreshBoard();
}
