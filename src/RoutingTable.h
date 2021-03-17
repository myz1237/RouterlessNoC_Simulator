/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     RoutingTable.h                                                          *
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

typedef struct RoutingTable{

    int node_id;   /*ID of destination*/
    int ring1_id;  /*Shortest path*/
    int ring1_hop;
    int ring2_id;  /*Second choice, may be the same as the ring1_id*/
    int ring2_hop;
}RoutingTable;

/**
 * @brief Record routing msg during the journey of a control packet
 */
typedef struct Routingsnifer {

    int node_id;   /*Current node where the control packet arrives*/
    int hop_count; /*Distance from the source to this specific node*/
}Routingsnifer;

/**
 * @brief Update the current routing table when receiving one routing msg from a control packet
 * @param ring_id Id of the ring where the node receive a control packet
 */
void update_routing_table(vector<Routingsnifer*>&snifer, vector<RoutingTable*>&temp, int ring_id);
void swap(Routingsnifer* snifer, RoutingTable* table, int ring_id);
#endif //NOCSIM_ROUTINGTABLE_H
