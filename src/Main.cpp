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
/**/
#define CONSOLE_OUPTUT 1

void config_output();
void run();

int main() {
    //Log System Initial
#if CONSOLE_OUPTUT
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
#else
    plog::init(plog::debug, "../Log/runtime.txt", 1000000);
#endif
    //plog::init<1>(plog::debug, "../Log/transposesecondresult.txt", 1000000);
    plog::init<1>(plog::debug, "../Log/Intensive_Hotspot_Max&321.txt", 1000000);
    //plog::init<1>(plog::debug, "../Log/validate.txt", 1000000);

    /*Read Parameters from Yaml File*/

/*
    configure();
    run();
*/

/*TODO 记得运行前 改成5 flit的packet 注释掉configuration里面traffic的代码 记得文件名也改一下*/

    int node_sum = 64;

/*    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Uniform;
        GlobalParameter::traffic = new TrafficUniform(node_sum);
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }*/

/*    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = BitReverse;
        GlobalParameter::traffic = new TrafficBitReverse(node_sum);
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Uniform;
        GlobalParameter::traffic = new TrafficUniform(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = BitReverse;
        GlobalParameter::traffic = new TrafficBitReverse(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Uniform;
        GlobalParameter::traffic = new TrafficUniform(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 1;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 1;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    *//*这一部分还没有跑完*//*
    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = BitReverse;
        GlobalParameter::traffic = new TrafficBitReverse(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 1;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }*/

/*    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 3;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }*/

/*    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 1;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }*/


/*    for(int i = 0; i < 24; i++){
        configure();
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }

    for(int i = 0; i < 24; i++){
        configure();
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 3;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }*/

/*
    for(int i = 0; i < 3; i++){
        configure();
        GlobalParameter::ej_port_nu = 1;
        GlobalParameter::exb_num += i;
        run();
    }

    for(int i = 0; i < 3; i++){
        configure();
        GlobalParameter::ej_port_nu = 2;
        GlobalParameter::exb_num += i;
        run();
    }

    for(int i = 0; i < 3; i++){
        configure();
        GlobalParameter::ej_port_nu = 3;
        GlobalParameter::exb_num += i;
        run();
    }


    configure();
    GlobalParameter::ej_port_nu = 16;
    GlobalParameter::exb_num = 16;
    run();
*/

/*    for(int i = 0; i < 46; i++){
        configure();
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }*/
/*    for(int i = 0; i < 46; i++) {
        configure();
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 3;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.005 * i;
        run();
    }*/
/*    for(int i = 1; i < 11; i++){
        configure();
        GlobalParameter::packet_size_with_ratio[0] = make_pair(i,2);
        GlobalParameter::exb_size = i;
        run();
    }*/

    /*0.01-0.5*/
/*
    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Uniform;
        GlobalParameter::traffic = new TrafficUniform(node_sum);
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Uniform;
        GlobalParameter::traffic = new TrafficUniform(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 3;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Uniform;
        GlobalParameter::traffic = new TrafficUniform(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Uniform;
        GlobalParameter::traffic = new TrafficUniform(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 1;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    */
/*0.01-0.30*//*

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 3;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = Transpose;
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 1;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }


    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = BitReverse;
        GlobalParameter::traffic = new TrafficBitReverse(node_sum);
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = BitReverse;
        GlobalParameter::traffic = new TrafficBitReverse(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 3;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 30; i++){
        configure();
        GlobalParameter::traffic_type = BitReverse;
        GlobalParameter::traffic = new TrafficBitReverse(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }
*/

    for(int i = 0; i < 12; i++){
        configure();
        GlobalParameter::traffic_type = Hotspot;
        GlobalParameter::traffic = new TrafficHotspot(node_sum);
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 12; i++){
        configure();
        GlobalParameter::traffic_type = Hotspot;
        GlobalParameter::traffic = new TrafficHotspot(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 3;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 12; i++){
        configure();
        GlobalParameter::traffic_type = Hotspot;
        GlobalParameter::traffic = new TrafficHotspot(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 2;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

    for(int i = 0; i < 12; i++){
        configure();
        GlobalParameter::traffic_type = Hotspot;
        GlobalParameter::traffic = new TrafficHotspot(node_sum);
        GlobalParameter::exb_strategy = Avg;
        GlobalParameter::exb_size = 1;
        GlobalParameter::enable_interrupt = true;
        GlobalParameter::injection_rate += 0.02 * i;
        run();
    }

}

void config_output(){

    PLOG_INFO_(1) << "Warmup Cycle: " <<GlobalParameter::sim_warmup;
    PLOG_INFO_(1) << "Simulation Cycle: " <<GlobalParameter::sim_time - GlobalParameter::sim_warmup;
    PLOG_INFO_(1) << "Injection Rate: " <<GlobalParameter::injection_rate;
    PLOG_INFO_(1) << "The number of ejection link: " << GlobalParameter::ej_port_nu;
    PLOG_INFO_(1) << "The number of EXB: " << GlobalParameter::exb_num;
    PLOG_INFO_(1) << "EXB Size: " << GlobalParameter::exb_size;
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == Uniform) << "Traffic Type: " << "Uniform";
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == Transpose) << "Traffic Type: " << "Transpose";
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == BitReverse) << "Traffic Type: " << "BitReverse";
    PLOG_INFO_IF_(1,GlobalParameter::traffic_type == Hotspot) << "Traffic Type: " << "Hotspot";
    PLOG_INFO_IF_(1,GlobalParameter::method_size_generator == 1) << "Packet Size: " << "Customized";
    PLOG_INFO_IF_(1,GlobalParameter::method_size_generator == 0) << "Packet Size: " << "Random";
    PLOG_INFO_IF_(1,GlobalParameter::exb_strategy == Max) << "EXB Strategy: " << "Max";
    PLOG_INFO_IF_(1,GlobalParameter::exb_strategy == Avg) << "EXB Strategy: " << "Avg";
    PLOG_INFO_IF_(1,GlobalParameter::routing_strategy == Shortest) << "Routing Strategy: " << "Shortest";

}

void run(){
    /*Set system time as a seed*/
    //random_seed(time(NULL));
    random_seed(1);
    config_output();
    Noc* c = new Noc;
    c->run();
    delete c;
    c = nullptr;
}
