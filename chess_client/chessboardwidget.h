#ifndef CHESSBOARDWIDGET_H
#define CHESSBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QJsonObject>
#include <QJsonArray>

class ChessBoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChessBoardWidget(QWidget *parent = nullptr);

public slots:
    void updateBoard(const QJsonObject &boardJson);
    void setSelection(int x, int y, bool enabled);

signals:
    void squareClicked(int x, int y);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel* squares[8][8];
    QString pieceToUnicode(const QString &type, bool white);
    void updateSquareGeometry();
};

#endif // CHESSBOARDWIDGET_H
