#include "hauptmenue.h"
#include "ui_hauptmenue.h"
#include "iostream"
#include "historiescreen.h"
#include "QLineEdit"
#include "suchscreen.h"

HauptMenue::HauptMenue(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HauptMenue)
{
    ui->setupUi(this);
    show();
}

HauptMenue::~HauptMenue()
{
    delete ui;
}

void HauptMenue::on_btnHistorieAnzeigen_clicked()
{
    QString spielername = ui->tfSpielername->text();
    HistorieScreen *h = new HistorieScreen(spielername, this);
    h->exec();
}

void HauptMenue::on_btnSpielSuchen_clicked()
{
    QString spielername = ui->tfSpielername->text();
    SuchScreen *s = new SuchScreen(spielername, this);
    s->show();
}

