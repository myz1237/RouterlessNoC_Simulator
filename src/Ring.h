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


//在每个cycle开始的时候就更新current node
    void update_curr_hop();

    inline int get_ring_id()const{return m_ring_id;}
    //检查该ring上有没有flit的currentnode是该nodeid的
    //返回packet在数组中的位置，以及flit在该packet的位置
    //把Flit指针返回
    Flit* flit_check(int node_id);
    //让packet上ring 由ring来控制
    void attach(Packet* p);
    //去掉该id的packet并清空所占内存
    void dettach(int packet_id);
    //ring上是否还有packet 为空返回true
    inline bool is_empty(){return m_packet.empty();}

    int find_packet_length(int packet_id);
    //static inline void initializer_ring(const vector<RingTopologyTuple*>& ring_tuple_set);
    Ring(int ring_id, RingTopologyTuple *ring_tuple, vector<Node*>& node);
    //TODO 只有最后模拟结束才清理ring中还没有ejection的packet
    ~Ring();

    //TODO 两个测试函数
    void print_ring_order();
    void print_packet_info();

private:
    int m_ring_id;
    //存储每个在该ring上的packet
    vector<Packet*>m_packet;
    //存储每一个ring经过的node节点的序列
    vector<int> m_ring_node_order;

    int find_next_node(int curr_node);


};


#endif //NOCSIM_RING_H