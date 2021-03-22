/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Noc.h                                                          *
*  @brief    Abstract definition of NoC.                                     *
*            It consists of all nodes in a NoC and provides essential        *
*            drive functions                                                 *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_NOC_H
#define NOCSIM_NOC_H
#include "RoutingTable.h"
#include "Node.h"
#include "Ring.h"
#include "ringalgorithm/RingAlgorithms.h"
#include "GlobalParameter.h"
#include <vector>
using namespace std;

/**
 * @brief Class Noc Definition
 *        Link all nodes in the network and drive it
 */
class Noc {
public:

    void run();
    void test();
    Noc();
    ~Noc();

private:
    /*Size of NoC, equal to GlobalParameter::mesh_dim_x or mesh_dim_y*/
    int m_size;

    /*Statistics usage, correct the num of received packets during the simulation*/
    int packet_num_correction;
    /*Statistics usage, correct the num of received flits during the simulation*/
    int flit_num_correction;

    /*Node sets*/
    vector<Node *> m_node;

    /*Direct results of Ring Topology Algorithms*/
    vector<RingTopologyTuple*>m_tuple;

    /*Record Statistics Data*/
    Stat* m_stat;

    void initial();
    void init_ring();
    void init_routing_table();
    int cal_ring_num(int mesh_x)const;

    /**
     * @brief Gather all statistics from nodes after simulation finishes
     */
    void stat_gather();
    void reset_stat();

    /**
     * @brief Gather the number of on-ring packets
     *        and the number of packetinfo in injection queues during the warm_up stage
     *        Aim to figure out the exact number of received packets during the simulation
     */
    int stat_correction();

    void packet_tracer();

};

#endif //NOCSIM_NOC_H
