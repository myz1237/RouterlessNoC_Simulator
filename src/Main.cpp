#include <iostream>
#include "ConfigureManager.h"
#include "Noc.h"

//TODO Ejection的时候会不会发生 前面的flit没有竞争成功注入 后面的flit注入呢
int main() {
    configure();
    Noc* c = new Noc;
    c->run();
    delete c;
    c = nullptr;
}
