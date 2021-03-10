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
//Only used when control packets are sent, otherwise it is NULL

typedef struct Packetinfo {
    int src;
    int dst;
    int length;
    int ctime;
}Packetinfo;

enum FlitType{
    Header,
    Payload,
    Tail,
    //TODO Complete control flit unit, Automatically generate injection table
    Control
};
//TODO Is there any other status left?
enum FlitStatus{
    Injecting,
    Routing,
    Buffered,
    Ejected
};

class Coordinate{
public:
    int x;
    int y;
    Coordinate(){}
    Coordinate(int x, int y):x(x),y(y){}
    static int coor_to_id(const Coordinate& coor);
    static Coordinate id_to_coor(const int id);
    static void fix_range(Coordinate& dst);
};

//TODO 给Flit加一个指向packet的指针 方便后面flit找Packet
//TODO 思考发送control包时的current_node id 发送一个cycle 传送一个cycle到下一个node？还是一个cycle 发送后直接到下一个node了？
class Flit {

    friend class Packet;
public:
    inline void update_hop(){m_hop++;}
    inline int get_packet_id()const{return m_packet_id;}
    inline FlitType get_flit_type()const{return m_type;}
    inline vector<Routingsnifer*>& get_flit_routing(){return m_routing;}
    inline int get_flit_ctime()const{return m_ctime;}
    inline int get_flit_dst()const{return m_dst_id;}
    inline void set_flit_type(FlitStatus status){m_status = status;}
    inline void set_atime(int cycle){m_atime = cycle;}
    int calc_flit_latency()const;
    void update_routing_snifer();
    ~Flit();
private:
    const long m_packet_id;
    const int m_src_id;
    const int m_dst_id;
    const FlitType m_type;
    const int m_sequence; // Sequence Num, located the position of the flit in the packet 0--(length-1)
    const int m_ctime; //Creation Time, expressed in cycle

    int m_atime; //Arrive Time -1---Not Arrive
    int m_hop; //Record current hop count, max is 255
    int m_curr_node; //Current node of the flit. Update at the beginning of each cycle
    //int m_next_node; //Next node id
    FlitStatus m_status;
    //Store routing msg from various rings, only used when control packets are sent
    vector<Routingsnifer*> m_routing;

    Flit(const long packet_id, const int src, const int dst, const FlitType type, const int seq,
         const int ctime, int hop, int curr_node, int atime = -1):
         m_packet_id(packet_id),m_src_id(src), m_dst_id(dst),m_type(type),m_sequence(seq), m_ctime(ctime),
         m_atime(atime),m_hop(hop),m_curr_node(curr_node), m_status(Injecting){}

};


//提供每个flit中数值读取和设定的功能 提供统一的查询下一个node并存储在nextnode中
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
    //inline int get_flit_next_node(int index)const{return m_flit[index]->m_next_node;}
    inline int get_flit_hop(int index)const{return m_flit[index]->m_hop;}
    inline int get_flit_seq(int index)const{return m_flit[index]->m_sequence;}
    //TODO 小心 未来可能会出内存上的问题 暂时不用
    inline vector<Routingsnifer*>& get_flit_routing(){return m_flit[0]->m_routing;}

    inline void set_flit_atime(int index, int time){m_flit[index]->m_atime = time;}
    inline void set_flit_curr_node(int index, int node){m_flit[index]->m_curr_node = node;}
    //inline void set_flit_next_node(int index, int node){m_flit[index]->m_next_node = node;}
    inline void set_flit_status(int index, FlitStatus status){m_flit[index]->m_status = status;}

    int calc_packet_latency()const;
    double calc_avg_flit_latency();
    //Normal Packet Constructor
    Packet(long packet_id, int length, int src, int dst, int node,int ctime, bool finish = false);
    //Control Packet Constructor
    Packet(long packet_id, int length, int src, int ctime, bool finish = false);
    //Initialize packet by packetinfo
    Packet(long packet_id, int src, Packetinfo *packetinfo, bool finish = false);
    ~Packet();
private:
    //全局上识别packet的标志 同样也作为flit的id号
    const long m_packet_id;
    vector<Flit*>m_flit;
    const int m_length;
    const int m_src_id;
    const int m_dst_id;
    const int m_ctime;
    //只是用来产生packet的时候给flit赋予初值
    int m_curr_node;
    bool m_finish;

    void attach(Flit* flit);

};

//Output Packet
ostream& operator<<(ostream& out, Packet& p);
ostream& operator<<(ostream& out, Packetinfo& p);
#endif //NOCSIM_FLIT_H
