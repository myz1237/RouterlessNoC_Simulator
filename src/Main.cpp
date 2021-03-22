#include <iostream>
#include "ConfigureManager.h"
#include "Noc.h"
#include "Test.h"
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

//------------------------------------------------------------------------
// Debug Defines. Set 1 to print flood of information of Packet and EXB
// Console Output Defines. Set 1 to print tracking msg on the console
//------------------------------------------------------------------------
#define CONSOLE_OUPTUT 1

void config_output();
void run();

int main() {
    //Log System Initial
#if CONSOLE_OUPTUT
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
#else
    plog::init(plog::debug, "../Log/runtime", 1000000);
#endif
    //plog::init<1>(plog::debug, "../Log/transposesecondresult.txt", 1000000);
    plog::init<1>(plog::debug, "../Log/test.txt", 1000000);
    //plog::init<1>(plog::debug, "../Log/validate.txt", 1000000);

    /*Read Parameters from Yaml File*/
/*    configure();
    run();*/

    for(int i = 0; i < 9; i++){
        configure();
        GlobalParameter::injection_rate += 0.05 * i;
        run();
    }



}

void config_output(){
    PLOG_INFO_(1) << "Warmup Cycle: " <<GlobalParameter::sim_warmup;
    PLOG_INFO_(1) << "Simulation Cycle: " <<GlobalParameter::sim_time - GlobalParameter::sim_warmup;
    PLOG_INFO_(1) << "Injection Rate: " <<GlobalParameter::injection_rate;
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == Uniform) << "Traffic Type: " << "Uniform";
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == Transpose) << "Traffic Type: " << "Transpose";
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == BitReverse) << "Traffic Type: " << "BitReverse";
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == Hotspot) << "Traffic Type: " << "Hotspot";
    PLOG_INFO_IF_(1,GlobalParameter::method_size_generator == 1) << "Packet Size: " << "Two Size";
    PLOG_INFO_IF_(1,GlobalParameter::method_size_generator == 0) << "Packet Size: " << "Random";
    PLOG_INFO_IF_(1,GlobalParameter::exb_strategy == Max) << "EXB Strategy: " << "Max";
    PLOG_INFO_IF_(1,GlobalParameter::exb_strategy == Avg) << "EXB Strategy: " << "Avg";
    PLOG_INFO_IF_(1,GlobalParameter::routing_strategy == Shortest) << "Routing Strategy: " << "Shortest";
    PLOG_INFO_IF_(1,GlobalParameter::routing_strategy == Secondwinner) << "Routing Strategy: " << "Secondwinner";

}

void run(){
    /*Set system time as a seed*/
    random_seed(time(NULL));
    config_output();
    Noc* c = new Noc;
    c->run();
    //c->time_to_generate_packetinfor();
    delete c;
    c = nullptr;
}
