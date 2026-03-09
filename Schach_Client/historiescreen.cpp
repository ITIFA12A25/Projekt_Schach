#include "historiescreen.h"
#include "ui_historiescreen.h"

//Erstellt Historie Screen von Spieler
HistorieScreen::HistorieScreen(const QString &text, QWidget *parent) : QDialog(parent), ui(new Ui::HistorieScreen) , spielerName(text){
    ui->setupUi(this);
    ui->lblSpielername->setText(spielerName);
}

//Schließt Fenster
HistorieScreen::~HistorieScreen()
{
    delete ui;
}

//Schließt Fenster wenn HistorieScreen erstellt wird
void HistorieScreen::on_pushButton_clicked()
{
    this->close();
}

