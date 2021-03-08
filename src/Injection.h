#ifndef NOCSIM_INJECTION_H
#define NOCSIM_INJECTION_H
#include "Flit.h"
#include "ringalgorithm/RingAlgorithms.h"
#include "Util.h"
#include "GlobalParameter.h"
#include "Traffic.h"

#include <vector>
class Packet;
class Ring;
class Flit;
struct Packetinfo;
class Traffic;
class ExbManager;
using namespace std;

class Injection {
public:

    //Core function of Injection
    void run_injection(int cycle);
    //仅用于初始化injection，向所有穿过该节点的ring发送一个flit的包
    void controlpacket_generator(int cycle,vector<int>& curr_ring_id, vector<Ring*>& ring);

    inline void set_exb_interrupt(bool status){m_exb_interrupt = status;}

    Injection(int node_id, vector<int>* ring, vector<RoutingTable*>* table,
              vector<pair<int, int>>* ej_order, ExbManager* exb, bool status=false);
    ~Injection();

private:
    int m_local_id;

    //记录当前packet要发送到的ring在该Node里对应的index
    int m_packet_ring_id;

    //True--Injection ongoing False--No Injection
    bool m_injection_status;

    //Extension Buffer Interrupt
    bool m_exb_interrupt;

    Packet* m_ongoing_packet;

    //发送一个packet之后清除一个packetinfo 需要的时候才根据packetinfo产生packet
    vector<Packetinfo*> m_packetinfo;

    //Node Information
    vector<int>* m_curr_ring_id;
    vector<RoutingTable*>* m_table;
    //vector<Flit*>* m_single_buffer;
    vector<pair<int, int>>* m_ej_order;
    //Exb Manager
    //只调用其函数 并不负责他的创建和释放 归Node管理
    ExbManager* m_exb_manager;

    //Inject ongoing packet and new packet
    void injector();
    int ring_to_index(int ring_id);
    void try_to_inject();

    //如果这里检查src和dst相等 就把该packetinfo删除释放 也就是说该节点不会再产生任何packet
    void packetinfo_attach(Packetinfo* info);
    void packetinfo_generator(int cycle, Traffic& traffic);
    //packetinfo和packet他们的产生不同点在于 packetinfo需要严格按照产生速度
    // 而packet只要满足条件(ring+exb)且m_packetinfo不为空 就要产生packet
    //检查m_packetinfo不为空 就要产生packet 放在exb检查后面
    void packet_generator();

    //TODO 一定要在Node接受过一轮Flit之后调用
    RoutingTable * check_routing_table(int dst_id);
    int check_single_buffer_action(int ring_index);

    int ring_selection(int dst, int index);
};


#endif //NOCSIM_INJECTION_H
