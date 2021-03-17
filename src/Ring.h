/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Ring.h                                                          *
*  @brief    Rings are like electrical wires, receiving packets and carry    *
*            carry them to destinations. This file defines rings             *
*            initialization and specific functions                           *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_RING_H
#define NOCSIM_RING_H
#include <vector>
#include <string>
#include <algorithm>
#include "Flit.h"
#include "GlobalParameter.h"
#include "ringalgorithm/RingAlgorithms.h"
#include "Util.h"
#include "Node.h"

class Flit;
class Node;
class Packet;
class RingTopologyTuple;
using namespace std;
class Ring {

public:

    inline bool is_empty()const{return m_packet.empty();}

    /**
     * @brief  Stat how many packets are left after the warm up stage and simulation
     */
    inline int left_packet_num()const{return m_packet.size();}

    /**
     * @brief  Simulate wires' actions
     *         Move on-ring packets forward
     *         Only flits with 'Routing' Status move to the next node
     */
    void update_curr_hop();

    /**
     * @brief  Figure out whether flits arrive in your node
     * @return If so, return the pointer, or null
     */
    Flit* flit_check(int node_id);

    inline void attach(Packet* p){m_packet.push_back(p);}

    /**
     * @brief  Dettach a packet and delete the memory occupied
     */
    void dettach(long packet_id);

    int find_packet_length(long packet_id);

    /**
     * @brief  New a memory space for the ring
     *         Calculate all node ids crossed by one ring
     *         Attach its ring id to corresponding notes(m_curr_ring_id)
     *         In this way, each node know
     * @param ring_id
     * @param ring_tuple A pointer to ring topology results from Algorithm
     * @param node       Reference of the node set
     */
    Ring(int ring_id, RingTopologyTuple *ring_tuple, vector<Node*>& node);
    ~Ring();

    /*Only for test*/
    void print_ring_order();
    void print_packet_info();


private:
    int m_ring_id;
    /*On-ring Packet array*/
    vector<Packet*>m_packet;
    /*Node ids crossed by this ring*/
    vector<int> m_ring_node_order;
    /**
     * @brief  Tell on-ring packets where to go in the cycle
     */
    int find_next_node(int curr_node);

};


#endif //NOCSIM_RING_H
