#include "chessboardwidget.h"
#include <QMouseEvent>
#include <QFont>

ChessBoardWidget::ChessBoardWidget(QWidget *parent)
    : QWidget(parent)
{
    QFont font("Arial", 32);

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            QLabel *label = new QLabel(this);
            label->setAlignment(Qt::AlignCenter);
            label->setFont(font);

            bool dark = (x + y) % 2;
            label->setStyleSheet(dark ? "background:#769656;" : "background:#eeeed2;");

            squares[y][x] = label;
        }
    }
}

QString ChessBoardWidget::pieceToUnicode(const QString &type, bool white)
{
    if (type == "king")   return white ? "♔" : "♚";
    if (type == "queen")  return white ? "♕" : "♛";
    if (type == "rook")   return white ? "♖" : "♜";
    if (type == "bishop") return white ? "♗" : "♝";
    if (type == "knight") return white ? "♘" : "♞";
    if (type == "pawn")   return white ? "♙" : "♟";
    return "";
}

void ChessBoardWidget::updateBoard(const QJsonObject &boardJson)
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            squares[y][x]->setText("");

    QJsonArray positions = boardJson["positions"].toArray();

    for (const QJsonValue &v : positions) {
        QJsonObject pos = v.toObject();
        int x = pos["x"].toInt();
        int y = pos["y"].toInt();

        if (!pos["piece"].isNull()) {
            QJsonObject piece = pos["piece"].toObject();
            QString type = piece["type"].toString();
            bool white = piece["white"].toBool();
            squares[y][x]->setText(pieceToUnicode(type, white));
        }
    }
}

void ChessBoardWidget::setSelection(int x, int y, bool enabled)
{
    bool dark = (x + y) % 2;
    QString base = dark ? "background:#769656;" : "background:#eeeed2;";

    if (enabled)
        squares[y][x]->setStyleSheet(base + "border: 3px solid yellow;");
    else
        squares[y][x]->setStyleSheet(base);
}

void ChessBoardWidget::resizeEvent(QResizeEvent *)
{
    updateSquareGeometry();
}

void ChessBoardWidget::updateSquareGeometry()
{
    int side = qMin(width(), height());
    int cell = side / 8;

    int offsetX = (width() - side) / 2;
    int offsetY = (height() - side) / 2;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            squares[y][x]->setGeometry(
                offsetX + x * cell,
                offsetY + y * cell,
                cell,
                cell
                );
        }
    }
}

void ChessBoardWidget::mousePressEvent(QMouseEvent *event)
{
    int side = qMin(width(), height());
    int cell = side / 8;

    int offsetX = (width() - side) / 2;
    int offsetY = (height() - side) / 2;

    int x = (event->position().x() - offsetX) / cell;
    int y = (event->position().y() - offsetY) / cell;

    if (x >= 0 && x < 8 && y >= 0 && y < 8)
        emit squareClicked(x, y);
}
