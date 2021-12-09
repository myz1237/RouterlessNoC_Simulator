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
#include "Ring.h"
#include <vector>
#include <algorithm>
class Ring;
using namespace std;

typedef struct RoutingTable{

    int node_id;                     /*ID of destination*/
    int routing_index;               /*Point to the next Routing Info*/
    vector<pair<int, int>> routing;  /*Routing Info: Ring ID + Hop Count*/

    void table_sort();               /*Sort Routing Info by Hop Count in the increasing order*/
    void reset_index(){routing_index = 0;}
    ~RoutingTable();
}RoutingTable;

/**
 * @brief Record routing msg during the journey of a control packet
 */
typedef struct Routingsnifer {

    int node_id;   /*Current node where the control packet arrives*/
    int hop_count; /*Distance from the source to this specific node*/
}Routingsnifer;

/**
 * @brief Update the current routing routing when receiving one routing msg from a control packet
 * @param ring_id Id of the ring where the node receive a control packet
 */
void update_routing_table(vector<Routingsnifer*>&snifer, vector<RoutingTable*>&temp, int ring_id);
static bool table_comp(pair<int, int>&a, pair<int, int>&b){return a.second < b.second;}
static bool table_comp_with_ring_size(pair<int, int>&a, pair<int, int>&b);
#endif //NOCSIM_ROUTINGTABLE_H
