#include "suchscreen.h"
#include "ui_suchscreen.h"
#include "QDialog"
SuchScreen::SuchScreen(const QString &text, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::SuchScreen),
    spielerName(text)
{
    ui->setupUi(this);
    ui->lblSpielername->setText(spielerName);
}

SuchScreen::~SuchScreen()
{
    delete ui;
}

void SuchScreen::on_btnAbrechen_clicked()
{
    this->close();
}

