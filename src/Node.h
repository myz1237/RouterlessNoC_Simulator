#ifndef NOCSIM_NODE_H
#define NOCSIM_NODE_H
#include <iostream>
#include <vector>
#include <cstring>
#include "Flit.h"
#include "RoutingTable.h"
#include "ExbManager.h"
#include "Injection.h"
#include "ExbManager.h"
class Injection;
class Flit;
class ExbManager;
using namespace std;

typedef struct Stat{
    int received_flit;
    int received_packet;
    int flit_delay;
    int packet_delay;
    int max_flit_delay;
    int max_packet_delay;

    //Node里面是不计算下面的两个值的
    double flit_throughput;
    double packet_throughput;
    double avg_flit_latency;
    double avg_packet_latency;

    void reset(){
        memset(this, 0, sizeof(Stat));
    }
}Stat;

class Node {
public:

    Injection* m_inject;
    vector<int>m_curr_ring_id;//存储穿过该node的ringid号
    vector<RoutingTable*>m_table;
    //TODO 应不应该存到public呢 还是private呢
    ExbManager* m_exb_manager;


    //Output Function
    void node_info_output();

    // Getter Function
    inline int get_received_flit()const{return m_stat.received_flit;}
    inline int get_received_packet()const{return m_stat.received_packet;}
    inline int get_flit_delay()const{return m_stat.flit_delay;}
    inline int get_packet_delay()const{return m_stat.packet_delay;}
    inline int get_max_flit_delay()const{return m_stat.max_flit_delay;}
    inline int get_max_packet_delay()const{return m_stat.max_packet_delay;}
    inline int get_node_id()const{return m_node_id;}

    //Stat
    void reset_stat();

    //添加穿过该node的ring的id号
    inline void set_curr_ring(int ring_id){m_curr_ring_id.push_back(ring_id);}
    void handle_all_single_buffer();
    void handle_control_packet();
    void inj_and_ej();
    void ej_control_packet();
    void init();
    Node(int node_id);
    ~Node();

private:
    const int m_node_id;

    //Statistics
    Stat m_stat;

    //TODO 暂时不用ej_link 等后面引入为某个packet留link 先简单实现一下
    //vector<bool>m_ej_link;
    vector<int>m_is_ej_idle;
    //数组长度是穿过该node的数目
    vector<Flit*>m_single_buffer;
    //第一个存储本来的次序之后用来得到ring_id和single_buffer
    vector<pair<int, int>>m_ej_order;

    void update_flit_stat(int latency);
    void update_packet_stat(int latency);

    void recv_flit();

    //EXB-related Function
    void check_exb(int single_buffer_index);

    //专门设计用来接收ejection的

    void ejection(Flit* flit, int ring_id);
    void forward(Flit* flit, int ring_id, int single_buffer_index);

    void reset_single_buffer();
    void get_ej_order();

    static bool comp(pair<int, int>&a, pair<int, int>&b);



};
ostream& operator<<(ostream& out, Node& node);
ostream& operator<<(ostream& out, Stat& stat);
#endif //NOCSIM_NODE_H
