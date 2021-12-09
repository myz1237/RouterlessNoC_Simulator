/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Node.h                                                          *
*  @brief    This file defines what modules one node should have.            *
*            Instantiated NoC and schedule injection, extensiont buffer,     *
*            ejection and rings across itself                                *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_NODE_H
#define NOCSIM_NODE_H
#include <iostream>
#include <vector>
#include <cstring>
#include <plog/Log.h>
#include "Flit.h"
#include "RoutingTable.h"
#include "ExbManager.h"
#include "Injection.h"
#include "ExbManager.h"
class Injection;
class Flit;
class ExbManager;
class RoutingTable;
using namespace std;

/**
 * @brief Statistics Structure
 */
typedef struct Stat{
    int received_flit;
    int received_packet;
    long flit_delay;
    long packet_delay;
    int max_flit_delay;
    int max_packet_delay;
    long packet_id_for_max_delay; /*Record the id of packet with max delay*/


    double flit_throughput;       /*Calculate these data only in Noc object, not node*/
    double packet_throughput;     /*Calculate these data only in Noc object, not node*/
    double avg_flit_latency;      /*Calculate these data only in Noc object, not node*/
    double avg_packet_latency;    /*Calculate these data only in Noc object, not node*/

    void reset(){
        memset(this, 0, sizeof(Stat));
    }

}Stat;

/**
 * @brief Class Node definition
 *        Basic unit of NoC
 *        Integrate all modules, input buffer, injection, ejection and EXB
 */
class Node {
public:

    vector<RoutingTable*>m_table;

    /*Getter functions*/
    inline int get_received_flit()const{return m_stat.received_flit;}
    inline int get_received_packet()const{return m_stat.received_packet;}
    inline long get_flit_delay()const{return m_stat.flit_delay;}
    inline long get_packet_delay()const{return m_stat.packet_delay;}
    inline int get_max_flit_delay()const{return m_stat.max_flit_delay;}
    inline int get_max_packet_delay()const{return m_stat.max_packet_delay;}
    inline long get_packet_id_for_max_delay()const{return m_stat.packet_id_for_max_delay;}

    /**
     * @brief Attach ids of rings across this node to the vector
     */
    inline void set_curr_ring(int ring_id){m_curr_ring_id.push_back(ring_id);}

    /**
     * @brief  Main functions to run this node
     *         Running Process:
     *                         Generate a new packetinfo and put it into the injection queue
     *                         Receive arrival flits from rings
     *                         Read data of flits and get the ejection order
     *                         Injection Packets and prioritize in the corresponding single buffer
     *                         Handle rest of single buffers
     *                         Clear single buffers for the next cycle
     * @param  cycle Time
     */
    void run(long cycle);
    void sort_routing_table();

    /**
     * @brief Clear statistics data of the node
     */
    void reset_stat();

    /**
     * @brief Special functions only to inject Control Packet
     *        Injection Pimpl
     */
    void inject_control_packet();

    /**
     * @brief A public interface for handling control packets
     *        Process:
     *                Receive-->Arbitrator-->Eject
     */
    void handle_control_packet();

    /**
     * @brief return the number of packetinfo in the injection queue
     *        Injection Pimpl
     */
    int left_injection_queue_packet()const;
    int left_injection_queue_flit()const;
    void init();

    Node(int node_id);
    ~Node();

    /*Only for Test*/
    void node_info_output();

    /**
     * @brief Print Details of Routing Table
     *        To enable this function, set 1 to the variable, sim_detail in Yaml Flie
     */
    void print_routing_table();

private:

    const int m_node_id;

    /* Count how many times a self-loop packet tries to find an idle ring
     * Fail to inject when the counter is equal to the number of rings across this node,
     * meaning that all rings are busy now.
     * */
    int m_self_loop_counter;

    /*Record the number of flit to be forwarded when interruption occurs*/
    int m_forward_exb_length;

    Stat m_stat;

    /*A pointer to implement EXB functions*/
    ExbManager* m_exb_manager;

    /*A pointer to implement Injection functions*/
    Injection* m_inject;

    /* Record IDs of rings across this node
     * Get to know which rings cross this node
     * Init in Ring Constructor
     * */
    vector<int>m_curr_ring_id;

    /* Input buffer, called single buffer
     * One single buffer, corresponding to one ring (m_curr_ring_id)
     * You may use the same index to access m_curr_ring_id & m_single_buffer
     * eg:
     *    The ring id of m_single_buffer[0] is m_curr_ring_id[0]
     * */
    vector<Flit*>m_single_buffer;

    /* Get ejection orders of multiple single buffers
     * Node's modules will follow this order to go actions
     * Ejection order:
     *                pair<first, second>
     *                First: Index of m_single_buffer
     *                Second: creation time of each flit stored in a single buffer, also called action
     *                        -3 --> Single buffer to be ejected
     *                        -2 --> Empty single buffer, no flit received in this cycle
     *                        -1 --> Single buffer to be forwarded
     *                        >=0--> Single buffer to be forwarded due to limited ejection links,
     *                               though the inner flit arrives its destination
     * */
    vector<pair<int, int>>m_ej_order;

    /* Ejection record of long packets(more than 1 flit):
     *                 pair<first, second>
     *                 First:  Packet ID of received long packet
     *                 Second: Expected Sequence Number of next flit
     * */
    vector<pair<long, int>>m_ej_record;

    inline void reset_self_loop_counter(){m_self_loop_counter = 0;}
    void reset_routing_table_index();

    /*Gather Statistics of received flits or packets*/
    void update_flit_stat(int latency);
    void update_packet_stat(int latency, long packet_id);

    /**
     * @brief Handle all arrival control packets, eject or forward
     *        To accelerate routing tables generation, here we do not follow the normal ejection process.
     *        All control packets will be ejected or forwarded in one cycle
     */
    void ej_fwd_control_packet();

    /**
     * @brief Check whether single buffers receive flits in this cycle
     *        GlobalParameter::ring Pimpl
     */
    void recv_flit();

    /**
     * @brief Ejection Action
     *        Handle Control, Header, Payload, and Tail Packet
     *        Update hop counts, status, ejection records, and erase completely received packets
     * @param flit Flit to be ejected
     * @param ring_id IDs for global rings, fetching through m_curr_ring_id[single buffer index]
     */
    void ejection(Flit* flit, int ring_id);

    /**
     * @brief Forward Action
     *        Update routing msg (Routing Snifer) in control packets
     *        No action to other three types of flits. Just keep it status as Routing
     */
    void forward(Flit* flit);

    /**
     * @brief Clear all single buffers at the end of each cycle
     */
    void reset_single_buffer();

    /**
     * @brief Read values of received flits and get ejection orders
     *        Ejection Priority:
     *        Tail(on records) > Payload(on records) > Header
     *        If multiple flits with the same type, sort them with creation time.
     *        The oldest flits win
     *        Forwarding:
     *        Non-destination flits & loser flits in ejection
     */
    void ej_arbitrator();

    /**
     * @brief Check whether this packet id and sequence number are in the list or not
     */
    bool check_record(long packet_id, int seq);

    /**
     * @brief Update Ejection record according to flit types
     *        Header: Create a new record. Expected Sequence number is set 1
     *        Payload: Sequence number plus 1
     *        Tail: Finish and erase the current record
     */
    void update_record(long packet_id, int type);

    /**
     * @brief Handle the rest single buffers after the injection process
     * @param action Creation time of the flit, the second value stored in pair<int, int> m_ej_record
     * @param single_flit_index Input buffer index
     */
    void handle_rest_flit(int action, int single_flit_index);

    /**
     * @brief  Key functions in the node
     *         Responsible for injecting new packet and manipulating EXB
     *         The single buffer on the ring to be chosen for injecting new packet will be handled(Ejection/Forward)
     *         Pipeline Implementation if ExbStrategy Avg is confirmed
     * @return return index of the ring chosen to inject new pakcets
     *         return -1, if no injection occurs
     */
    int inject_eject();

    bool is_injection_ongoing();

    /**
     * @brief Continue to inject long packets, whose header flit has been injected in the last cycle
     *        Handle the single buffer on the ring to be chosen for injecting this packet
     */
    void continue_inject_packet();

    /**
     * @brief  Selection appropriate rings according to the destination
     * @param  dst   Destination of the packet
     * @return Directly return index of the ring in the current node
     *         Return -1, if all rings are busy, inject in the next cycle
     */
    int ring_selection(int dst);

    RoutingTable* search_routing_table(int dst_id);

    /**
     * @brief  Selection appropriate rings according to the destination
     * @param  ring_id    Retrieve from Routing Table. Id of global ring
     * @return ring_index Directly return index of the ring in the current node
     */
    int ring_to_index(int ring_id);

    /**
     * @brief  Find ejection order(Tag) of specific single buffer
     * @return Return the action, the second value of ejection order
     */
    int get_single_buffer_action(int ring_index);

    static bool comp(pair<int, int>&a, pair<int, int>&b){return a.second > b.second;}
};

ostream& operator<<(ostream& out, Stat& stat);
bool operator==(const pair<long, int>& a, const pair<long, int>& b);
#endif //NOCSIM_NODE_H
