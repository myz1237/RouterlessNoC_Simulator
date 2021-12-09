#include "RoutingTable.h"

void RoutingTable::table_sort() {
    sort(routing.begin(), routing.end(), table_comp);
    //sort(routing.begin(), routing.end(), table_comp_with_ring_size);
}

RoutingTable::~RoutingTable(){
    vector<pair<int, int>>().swap(routing);
}

void update_routing_table(vector<Routingsnifer*>&snifer, vector<RoutingTable*>&temp, int ring_id){
    int counter = 0;
    for(int i = 0;i < snifer.size(); i++){
        counter = 0;
        for(int j = 0; j< temp.size(); j++){
            /*Check whether there has been a record about this node*/
            if(snifer[i]->node_id == temp[j]->node_id){
                temp[j]->routing.emplace_back(make_pair(ring_id, snifer[i]->hop_count));
                break;
            }
            counter++;
        }
        /*New node, add it to the routing*/
        if(counter == temp.size()){

            RoutingTable *p = new RoutingTable;
            p->routing_index = 0;
            p->node_id = snifer[i]->node_id;
            p->routing.emplace_back(make_pair(ring_id, snifer[i]->hop_count));
            temp.push_back(p);
        }
    }

}

static bool table_comp_with_ring_size(pair<int, int>&a, pair<int, int>&b){
    int test_1 = a.second;
    int test_2 = b.second;
    vector<Ring*>& ring = GlobalParameter::ring;
    if(test_1 != test_2) return test_1 < test_2;
    if(test_1 == test_2) return ring[a.first]->get_ring_size() < ring[b.first]->get_ring_size();
}
