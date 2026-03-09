#include "suchscreen.h"
#include "ui_suchscreen.h"
#include "QDialog"

//Such Screen wird erstellt.
SuchScreen::SuchScreen(const QString &text, QWidget *parent):
    QDialog(parent),
    ui(new Ui::SuchScreen),
    spielerName(text) {
        ui->setupUi(this);
        ui->lblSpielername->setText(spielerName);
    }

//schließt Fenster
SuchScreen::~SuchScreen() {
    delete ui;
}

//Funktion Button "Suche Abrechhen": bricht Ssuche ab
void SuchScreen::on_btnAbrechen_clicked() {
    this->close();
}

