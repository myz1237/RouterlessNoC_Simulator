
#ifndef NOCSIM_NOC_H
#define NOCSIM_NOC_H
#include "RoutingTable.h"
#include "Node.h"
#include "Ring.h"
#include "ringalgorithm/RingAlgorithms.h"
#include "GlobalParameter.h"
#include <vector>
using namespace std;

//TODO 还没有把时钟和packetID接入
class Noc {
public:


    void run();
    Noc();
    ~Noc();

private:

    //Node-Related stuff
    vector<Node *> m_node;
    int m_size;

    //Ring-Related stuff

    vector<RingTopologyTuple*>m_tuple;

    void initial();
    void init_ring();
    void init_routing_table();
    //Statistics
    void stat_gather();
    void reset_stat();
    int cal_ring_num(int mesh_x);



};

#endif //NOCSIM_NOC_H
