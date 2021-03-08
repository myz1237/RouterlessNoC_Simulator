#ifndef NOCSIM_GLOBALPARAMETER_H
#define NOCSIM_GLOBALPARAMETER_H
#include <string>
#include <iostream>
#include <vector>
#include "Ring.h"

class Traffic;
class RingAlgorithms;
class Ring;
using namespace std;
enum RingStrategy{
    IMR,
    RLrec
};

enum ExbStrategy{
    Max,
    Avg
};

enum EjStrategy{
    Oldest,
    Prio
};

enum TrafficType{
    Uniform,
    Transpose1,
    Transpose2,
    BitReverse,
    Hotspot,
    RequestandReply,
    SPLASH,
    PARSEC,
};

enum RoutingStrategy{
    Shortest,
    Secondwinner
};

enum SimType{
    Latency,
    Throughput
};

enum LatencyType{
    Packets,
    Flits,
    Both
};

namespace GlobalParameter{
    extern int mesh_dim_x;
    extern int mesh_dim_y;
    extern float injection_rate;
    extern int long_packet_size;
    extern int short_packet_size;
    extern int flit_size;
    extern int bandwidth;
    extern RingStrategy ring_strategy;
    //extern int ring_constraint;
    extern ExbStrategy exb_strategy;
    extern int exb_num;
    extern int in_port_size;
    extern int ej_port_nu;
    extern EjStrategy ej_strategy;
    extern RoutingStrategy routing_strategy;
    extern TrafficType traffic_type;
    extern vector<pair<int, double>>hotspot;
    extern int sim_time;
    extern int sim_warmup;
    extern SimType sim_type;
    extern LatencyType latency_type;
    extern int sim_detail;

    //GlobalParameter used during the project
    //injection_cycle = ceil(1/injection_rate)
    extern int injection_cycle;
    //static vector<RoutingTable*>routing_table;
    //static vector<Ring*>ring;
    extern Traffic* traffic;
    extern RingAlgorithms* ring_algorithm;
    //The size of buffer for each EXB
    extern int exb_size;
    extern long packet_id;
    extern vector<Ring*>ring;
    extern int global_cycle;
}

#endif //NOCSIM_GLOBALPARAMETER_H

