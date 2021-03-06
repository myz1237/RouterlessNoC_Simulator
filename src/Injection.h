/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Injection.h                                                          *
*  @brief    Injection is a part of Network Interface. This file             *
*            illustrates when and how to inject a flit and a packet.         *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_INJECTION_H
#define NOCSIM_INJECTION_H
#include "Flit.h"
#include "ringalgorithm/RingAlgorithms.h"
#include "Util.h"
#include "GlobalParameter.h"
#include "Traffic.h"

#include <vector>
#include <plog/Log.h>

class Packet;
class Ring;
class Flit;
struct Packetinfo;
class Traffic;
class ExbManager;
using namespace std;

/**
 * @brief Injeciton Module
 *        Generate New Packetinfo and Packets
 *        Flit(Data Flit) and Control Flit Injection
 *        Injection queue maintenance
 */
class Injection {
public:
    /*Getter and setter functions*/
    inline Packet* get_ongoing_packet()const{return m_ongoing_packet;}
    inline void complete_ongoing_packet(){m_ongoing_packet = nullptr;}
    inline int get_ongoing_ring_index()const{return m_injecting_ring_index;}
    inline bool is_injection_interrupted()const{return m_interrupt;}
    inline void set_interrupt(bool status){m_interrupt = status;}
    inline bool is_packetinfo_empty()const{return m_injection_queue.empty() ? true : false;}

    /**
     * @brief Return the oldest packetinfo to the node
     */
    inline Packetinfo* get_new_packetinfo()const{return m_injection_queue.front();}

    /**
     * @brief Get the size of injection queue when warmup ends
     */
    int left_packet_in_queue()const{return m_injection_queue.size();}

    /**
     * @brief Get the number of flits left when warmup ends
     */
    int left_flit_in_queue();
    /**
     * @brief Send control packet for each ring across the current node
     *        This function does not obey the sequential injection,
     *        but inject all control packets in one cycle,
     *        for accelerating the process of routing routing generation
     * @param cycle Current time
     * @param reference of ring_id vector of the current node.
     */
    void controlpacket_generator(const long cycle,vector<int>& curr_ring_id);

    /**
     * @brief Inject a new packet into the ring.
     *        If length is greater than 1,
     *        the function, Node::continue_inject_packet(), will inject the rest
     * @param ring_index  Get ring_id through access curr_ring_id[ring_index]
     */
    void inject_new_packet(int ring_index);

    /**
     * @brief Generate packetinfo according the time
     *        Process:
     *                 Get a packeinfor from Class Traffic
     *                 Extract length of packet to be injected from the packetinfo
     *                 Time difference*injection_rate > length?
     *                 -->Yes, put the packetinfo to the injeciton queue
     *                    Update the time cycle of this injection
     *                 -->No, wait for the next cycle
     *        Abandon the packetinfo, whose source and dst are the same.
     *        No much sense to inject a packet to the node itself.
     * @param traffic  reference of traffic object, determined in configuration phrase
     */
    void packetinfo_generator(const long cycle, Traffic& traffic);

    Injection(int node_id, vector<int>* curr_ring_id);
    ~Injection();

    /*Only for time_to_generate_packetinfor*/
    void print_packetinfo();

private:
    /*Node id*/
    int m_local_id;

    /* A container for the ring index of the being injected packet
     * Used in Node::continue_inject_packet()
     * */
    int m_injecting_ring_index;
    /* A pointer to the being injected packet
     * Also an indicator for whether injection is going on or not
     * */
    Packet* m_ongoing_packet;

    bool m_interrupt;

    /* Injection Queue
     * Always access the first element(the oldest one)
     * */
    vector<Packetinfo*> m_injection_queue;

    /*A pointer to ring_id vector stored in Node object*/
    vector<int>* m_curr_ring_id;

    /**
     * @brief Push back Packetinfo into the injeciton queue
     */
    void packetinfo_attach(Packetinfo* info, const long cycle);

};


#endif //NOCSIM_INJECTION_H
