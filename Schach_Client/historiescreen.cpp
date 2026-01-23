#include "historiescreen.h"
#include "ui_historiescreen.h"

HistorieScreen::HistorieScreen(const QString &text, QWidget *parent) : QDialog(parent), ui(new Ui::HistorieScreen) , spielerName(text){
    ui->setupUi(this);
    ui->lblSpielername->setText(spielerName);
}

HistorieScreen::~HistorieScreen()
{
    delete ui;
}

void HistorieScreen::on_pushButton_clicked()
{
    this->close();
}

