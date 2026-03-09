#include "controller.h"
#include "hauptmenue.h"

//Wenn Controller erzeugt wird, wird ein Hauptmenü Fenster erstellt
Controller::Controller() {
    hauptMenue = new HauptMenue();
}
