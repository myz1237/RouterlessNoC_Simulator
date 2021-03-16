#include <iostream>
#include "ConfigureManager.h"
#include "Noc.h"
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

//------------------------------------------------------------------------
// Debug Defines. Set 1 to print flood of information of Packet and EXB
//------------------------------------------------------------------------
#define DEBUG 0

int main() {
    //Log System Initial
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    /*Set system time as a seed*/
    //random_seed(time(NULL));

    /*Read Parameters from Yaml File*/
    configure();

    Noc* c = new Noc;
    c->run();

    delete c;
    c = nullptr;

}
