#ifndef HISTORIESCREEN_H
#define HISTORIESCREEN_H

#include <QWidget>
#include <QDialog>
namespace Ui {
class HistorieScreen;
}

class HistorieScreen : public QDialog
{
    Q_OBJECT

public:
    explicit HistorieScreen(const QString &text, QWidget *parent = nullptr);
    ~HistorieScreen();

private slots:
    void on_pushButton_clicked();

private:
    Ui::HistorieScreen *ui;
    QString spielerName;
};

#endif // HISTORIESCREEN_H
