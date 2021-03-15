#include <iostream>
#include "ConfigureManager.h"
#include "Noc.h"
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#define DEBUG 0

//TODO Ejection的时候会不会发生 前面的flit没有竞争成功注入 后面的flit注入呢
int main() {
    //Log System Initial
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    //Read Parameters from Yaml File
    configure();

    Noc* c = new Noc;
    c->run();

    delete c;
    delete GlobalParameter::traffic;
    c = nullptr;
    GlobalParameter::traffic = nullptr;
    cout<< GlobalParameter::error<<endl;
}
