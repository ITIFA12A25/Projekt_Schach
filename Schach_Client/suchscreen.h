#ifndef SUCHSCREEN_H
#define SUCHSCREEN_H

#include <QWidget>
#include <QDialog>
namespace Ui {
class SuchScreen;
}

class SuchScreen : public QDialog
{
    Q_OBJECT

public:
    explicit SuchScreen(const QString &text, QWidget *parent = nullptr);
    ~SuchScreen();

private slots:
    void on_btnAbrechen_clicked();

private:
    Ui::SuchScreen *ui;
    QString spielerName;
};

#endif // SUCHSCREEN_H
