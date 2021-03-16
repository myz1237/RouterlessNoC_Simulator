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
    Transpose,
    BitReverse,
    Hotspot,
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
    extern double injection_rate;
    extern int long_packet_size;
    extern int short_packet_size;
    extern int method_size_generator;
    extern int long_packet_ratio;
    extern int short_packet_ratio;
    extern RingStrategy ring_strategy;
    extern ExbStrategy exb_strategy;
    extern int exb_num;
    extern int ej_port_nu;
    extern EjStrategy ej_strategy;
    extern RoutingStrategy routing_strategy;
    extern TrafficType traffic_type;
    extern vector<pair<int, int>>hotspot;
    extern int sim_time;
    extern int sim_warmup;
    extern int sim_detail;

    extern Traffic* traffic;
    extern RingAlgorithms* ring_algorithm;
    extern int exb_size;
    extern long packet_id;
    extern vector<Ring*> ring;
    extern long global_cycle;
    extern bool enable_interrupt;
    extern int unrecv_packet_num;
}


#endif //NOCSIM_GLOBALPARAMETER_H

