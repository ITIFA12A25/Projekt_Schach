#ifndef HAUPTMENUE_H
#define HAUPTMENUE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class HauptMenue;
}
QT_END_NAMESPACE

class HauptMenue : public QMainWindow
{
    Q_OBJECT

public:
    HauptMenue(QWidget *parent = nullptr);
    ~HauptMenue();

signals: void sucheGegnerRequest();

private slots:
    void on_btnHistorieAnzeigen_clicked();

    void on_btnSpielSuchen_clicked();

private:
    Ui::HauptMenue *ui;
};
#endif // HAUPTMENUE_H
