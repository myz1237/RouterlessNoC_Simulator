/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     GlobalParameter.h                                                          *
*  @brief    This file stores parameters read from ConfigureManager.h        *
*            Global variable definition                                      *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

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

/**
 * @brief Algorithm to generate ring's topology
 */
enum RingStrategy{
    IMR,    /*Less efficient Alogrithm. DEPRECATED*/
    RLrec   /*Proposed by a previous paper*/
};
/**
 * @brief Determine the size of each EXB
 */
enum ExbStrategy{
    Max,    /*Maximum size of one packet*/
    Avg     /*Algorithm mean of packet size, cooperating pipeline strategy */
};

enum TrafficType{
    Uniform,
    Transpose,
    BitReverse,
    Hotspot
};

enum RoutingStrategy{
    Shortest,      /*Always use the shortest path to the dst*/
    Secondwinner   /*Try the second choice if the shortest one unavailable*/
};

namespace GlobalParameter{
    /*Global Parameters from Yaml File*/
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
    extern RoutingStrategy routing_strategy;
    extern TrafficType traffic_type;
    extern vector<pair<int, int>>hotspot;
    extern int sim_time;
    extern int sim_warmup;
    extern int sim_detail;

    /*Global Parameter for inner usages*/
    extern Traffic* traffic;
    extern RingAlgorithms* ring_algorithm;
    extern int exb_size;
    /*Global packet id for all traffics*/
    extern long packet_id;
    /*Ring sets for NoC*/
    extern vector<Ring*> ring;
    /*Time counter, one flit per cycle*/
    extern long global_cycle;
    /*Switch for injection interrupt
     *True if ExbStrategy, avg, is chosen */
    extern bool enable_interrupt;
    /*Record the number of packets in rings or injection queues*/
    extern int unrecv_packet_num;
}


#endif //NOCSIM_GLOBALPARAMETER_H

