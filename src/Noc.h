
#ifndef NOCSIM_NOC_H
#define NOCSIM_NOC_H
#include "RoutingTable.h"
#include "Node.h"
#include "Ring.h"
#include "ringalgorithm/RingAlgorithms.h"
#include "GlobalParameter.h"
#include <vector>
using namespace std;


class Noc {
public:

    void initial();
    void run();
    Noc();
    ~Noc();

private:

    //Node-Related stuff
    vector<Node *> m_node;
    int m_size = GlobalParameter::mesh_dim_x;

    //Ring-Related stuff

    vector<RingTopologyTuple*>m_tuple;

    void init_ring();
    void init_routing_table();

    //Statistics
    void stat_gather();
    void reset_stat();



};

#endif //NOCSIM_NOC_H
