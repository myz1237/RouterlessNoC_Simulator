#include <iostream>
#include "ConfigureManager.h"
#include "Noc.h"
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>



//TODO Ejection的时候会不会发生 前面的flit没有竞争成功注入 后面的flit注入呢
int main() {
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
    configure();
    Noc* c = new Noc;
    c->run();
    delete c;
    c = nullptr;
}
