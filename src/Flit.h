/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Flit.h                                                          *
*  @brief    Packet and flit definitions                                     *                    *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_FLIT_H
#define NOCSIM_FLIT_H
#include <vector>
#include <ostream>
#include <string>
#include "GlobalParameter.h"
#include "Util.h"
#include "RoutingTable.h"
using namespace std;
class Packet;

/**
 * @brief Intermediate variable for Packet
 *        Create a packetinfo when times up
 *        Instantiate a packet object through a packetinfo when available injection
 */
typedef struct Packetinfo {
    long id;
    int src;
    int dst;
    int length;
    int ctime; /*Creation Time*/
}Packetinfo;


enum FlitType{
    Header,
    Payload,
    Tail,
    Control /*Only for Routing table generation*/
};

/**
 * @brief Key variables for the simulator, suggesting what to do in the next cycle
 */
enum FlitStatus{
    Injecting, /*Flit of Packet in injection module, ready to be sent*/
    Routing,   /*Any flit in this status will be moved by rings and finally reach the dst*/
    Buffered,  /*Buffered in an Exb, not influenced by rings*/
    Ejected    /*Flit has been ejected by the destinations*/
};

/**
 * @brief Class Flit Definition, the base class of Control flit
 *        Getter and setter functions, Calculate flit latency
 *        Virtual functions to be overwritten for routing infor gathering
 *        3 Flit Type used in flit: Header, Payload, and Tail
 */
class Flit {

    friend class Packet;
public:
    inline void update_hop(){m_hop++;}
    inline long get_packet_id()const{return m_packet_id;}
    inline FlitType get_flit_type()const{return m_type;}
    inline int get_flit_ctime()const{return m_ctime;}
    inline int get_flit_dst()const{return m_dst_id;}
    inline int get_sequence()const{return m_sequence;}
    inline int get_hop()const{return m_hop;}
    inline void set_flit_status(FlitStatus status){ m_status = status;}
    inline void set_atime(int cycle){m_atime = cycle;}
    int calc_flit_latency()const;

    virtual void update_routing_snifer() {}
    virtual vector<Routingsnifer*>& get_flit_routing(){}

    Flit(const long packet_id, const int src, const int dst, const FlitType type, const int seq,
         const int ctime, int hop, int curr_node, int atime = -1);
    virtual ~Flit(){}

    const long m_packet_id;
    const int m_src_id;
    const int m_dst_id;
    const FlitType m_type;
    const int m_sequence; /*Sequence Num, located the position of the flit in the packet 0--(length-1)*/
    const int m_ctime;    /*Creation Time, expressed in cycle*/

    int m_atime;          /*Arrive Time Default Value: -1---Not Arrive*/
    int m_hop;            /*Record current hop count*/
    int m_curr_node;      /*Current node of the flit. Update at the beginning of each cycle*/
    FlitStatus m_status;  /*Store routing msg from various rings, only used when control packets are sent*/

};

/**
 * @brief Class Control Definition, inheriting Class Flit
 *        Overwrite routing-related functions
 *        Source and destination are set the same, which means each control flit round the ring and go back.
 */
class ControlFlit: public Flit{
public:

    inline vector<Routingsnifer*>& get_flit_routing(){return m_routing;}

    /**
    * @brief Record current hop count and node id when control flits arriving a non-destination node
    */
    void update_routing_snifer();

    ControlFlit(const long packet_id, const int src, const int dst, const FlitType type, const int seq, const int ctime,
                int hop, int curr_node);
    ~ControlFlit();

private:
    /*Record routing msg in each control packet*/
    vector<Routingsnifer*> m_routing;
};

/**
 * @brief Class Packet Definition. Each packet has several flits.
 *        Control flit movements from an upper view
 */
class Packet{

public:

    inline long get_id()const{return m_packet_id;}
    inline int get_length()const{return m_length;}
    inline int get_src()const{return m_src_id;}
    inline int get_dst()const{return m_dst_id;}
    inline int get_ctime()const{return m_ctime;}

    inline Flit* get_flit(int index)const{return m_flit[index];}
    inline FlitType get_flit_type(int index)const{return m_flit[index]->m_type;}
    inline FlitStatus get_flit_status(int index){return m_flit[index]->m_status;}
    inline int get_flit_atime(int index)const{return m_flit[index]->m_atime;}
    inline int get_flit_curr_node(int index)const{return m_flit[index]->m_curr_node;}
    inline int get_flit_hop(int index)const{return m_flit[index]->m_hop;}

    inline void set_flit_curr_node(int index, int node){m_flit[index]->m_curr_node = node;}
    inline void set_flit_status(int index, FlitStatus status){m_flit[index]->m_status = status;}

    /**
    * @brief Normal Packet Constructor, hardly use
    */
    Packet(long packet_id, int length, int src, int dst, int node,int ctime);

    /**
    * @brief Control Packet Constructor
    *        Only one flit per control packet
    *        Instantiate control flit object at the same time
    */
    Packet(long packet_id, int length, int src, int ctime);

    /**
    * @brief Instantiate and initialize packet through packetinfo
    *        All packet objects except control packet are created in this way
    *        Instantiate flit object at the same time
    */
    Packet(int src, Packetinfo *packetinfo);
    ~Packet();

private:

    const long m_packet_id;
    const int m_length;
    const int m_src_id;
    const int m_dst_id;
    const int m_ctime;
    int m_curr_node;
    /*Store and manipulate flits*/
    vector<Flit*>m_flit;

    /**
    * @brief Attach one flit into the array m_flit
    */
    void attach(Flit* flit);

};
/*Output packet information*/
ostream& operator<<(ostream& out, Packet& p);
/*Output packetinfo information*/
ostream& operator<<(ostream& out, Packetinfo& p);

#endif //NOCSIM_FLIT_H
