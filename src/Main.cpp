#include <iostream>
#include "ConfigureManager.h"
#include "Noc.h"
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#define DEBUG 0

int main() {
    //Log System Initial
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    random_seed(time(NULL));

    //Read Parameters from Yaml File
    configure();

    Noc* c = new Noc;
    c->run();

    delete c;
    delete GlobalParameter::traffic;
    c = nullptr;
    GlobalParameter::traffic = nullptr;
}
