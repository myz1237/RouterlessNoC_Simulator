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

class Injection {
public:

    inline Packet* get_ongoing_packet()const{return m_ongoing_packet;}
    inline void complete_ongoing_packet(){m_ongoing_packet = nullptr;}
    inline int get_ongoing_ring_index()const{return m_injecting_ring_index;}

    //仅用于初始化injection，向所有穿过该节点的ring发送一个flit的包
    void controlpacket_generator(int cycle,vector<int>& curr_ring_id);
    void inject_new_packet(int ring_index);
    bool is_packetinfo_empty();
    Packetinfo* get_new_packetinfo();
    void packetinfo_generator(int cycle, Traffic& traffic);

    //Only for test
    void print_packetinfo();

    Injection(int node_id, vector<int>* curr_ring_id);
    ~Injection();


private:
    int m_local_id;

    //记录当前packet要发送到的ring在该Node里对应的index
    int m_injecting_ring_index;

    //Extension Buffer Interrupt

    Packet* m_ongoing_packet;

    //发送一个packet之后清除一个packetinfo 需要的时候才根据packetinfo产生packet
    vector<Packetinfo*> m_packetinfo;

    //Node Information
    vector<int>* m_curr_ring_id;

    //如果这里检查src和dst相等 就把该packetinfo删除释放 也就是说该节点不会再产生任何packet
    void packetinfo_attach(Packetinfo* info, int cycle);

    //packetinfo和packet他们的产生不同点在于 packetinfo需要严格按照产生速度
    // 而packet只要满足条件(ring+exb)且m_packetinfo不为空 就要产生packet
    //检查m_packetinfo不为空 就要产生packet 放在exb检查后面

};


#endif //NOCSIM_INJECTION_H
