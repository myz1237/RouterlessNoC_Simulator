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
#define CONSOLE_OUPTUT 1

int main() {
    //Log System Initial
#if CONSOLE_OUPTUT
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
#else
    plog::init(plog::debug, "../Log/runtime.txt", 1000000);
#endif
    plog::init<1>(plog::debug, "../Log/result.txt", 1000000);
    /*Set system time as a seed*/
    //random_seed(time(NULL));

    /*Read Parameters from Yaml File*/
    configure();

    Noc* c = new Noc;
    c->run();

    delete c;
    c = nullptr;

}
