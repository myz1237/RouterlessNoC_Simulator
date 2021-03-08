#include <iostream>
#include "ConfigureManager.h"
#include "Noc.h"

int main() {
    configure();
    Noc* c = new Noc;
    c->run();
    delete c;
    c = nullptr;
}
