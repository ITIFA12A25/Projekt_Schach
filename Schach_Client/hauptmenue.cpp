#include "hauptmenue.h"
#include "ui_hauptmenue.h"
#include "iostream"
#include "historiescreen.h"
#include "QLineEdit"
#include "suchscreen.h"
//Klasse Hhauptmenü
HauptMenue::HauptMenue(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HauptMenue)
{
    ui->setupUi(this);
    show();
}
//Löscht ui
HauptMenue::~HauptMenue()
{
    delete ui;
}
//Button Historie funktion
void HauptMenue::on_btnHistorieAnzeigen_clicked()
{
    QString spielername = ui->tfSpielername->text();
    HistorieScreen *h = new HistorieScreen(spielername, this);
    h->exec();
}
//Button Suche funktion
void HauptMenue::on_btnSpielSuchen_clicked()
{
    QString spielername = ui->tfSpielername->text();
    //Startet Fenster Spielersuche
    SuchScreen *s = new SuchScreen(spielername, this);
    s->show();
}

