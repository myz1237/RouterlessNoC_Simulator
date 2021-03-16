/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Flit.h                                                          *
*  @brief    Data structures for routing tables and how to calculate them    *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_ROUTINGTABLE_H
#define NOCSIM_ROUTINGTABLE_H

#include <vector>
#include <algorithm>

using namespace std;
//Routing Table for each node
typedef struct RoutingTable{
    int node_id;
    int ring1_id;// Shortest path
    int ring1_hop;
    int ring2_id;// Second Choice
    int ring2_hop;
}RoutingTable;

typedef struct Routingsnifer {
    //Current node where the control packet arrives
    int node_id;
    //Distance from the source to this specific node
    int hop_count;
}Routingsnifer;

void update_routing_table(vector<Routingsnifer*>&snifer, vector<RoutingTable*>&temp, int ring_id);
void swap(Routingsnifer* snifer, RoutingTable* table, int ring_id);
#endif //NOCSIM_ROUTINGTABLE_H
