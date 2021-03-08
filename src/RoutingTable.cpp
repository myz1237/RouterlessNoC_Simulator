#include "RoutingTable.h"

void update_routing_table(vector<Routingsnifer*>&snifer, vector<RoutingTable*>&temp, int ring_id){
    int counter = 0;
    for(int i = 0;i < snifer.size(); i++){
        //检查该temp里面有没有这个node_id
        counter = 0;
        for(int j = 0; j< temp.size(); j++){
            //有的话 应该只能找到一个匹配
            if(snifer.at(i)->node_id == temp.at(j)->node_id){
                swap(snifer.at(i),temp.at(j), ring_id);
                break;
            }
            counter++;
        }

        if(counter == temp.size()){
            //没找到 加入该节点
            RoutingTable *p = new RoutingTable;
            p->node_id = snifer.at(i)->node_id;
            p->ring1_id = ring_id;
            p->ring1_hop = snifer.at(i)->hop_count;
            p->ring2_id = ring_id;
            p->ring2_hop = snifer.at(i)->hop_count;
            temp.push_back(p);
        }

    }
}
void swap(Routingsnifer* snifer, RoutingTable* table, int ring_id){
    int test = snifer->hop_count;
    int ring1_hop = table->ring1_hop;
    int ring2_hop = table->ring2_hop;
    int ring1 = table->ring1_id;
    int ring2 = table->ring2_id;
    if(test <= ring1_hop){
        if(ring1_hop < ring2_hop){
            table->ring2_id = ring1;
            table->ring2_hop = table->ring1_hop;
            table->ring1_id = ring_id;
            table->ring1_hop = test;
        }else{
            table->ring1_id = ring_id;
            table->ring1_hop = test;
        }
    }else if(test < ring2_hop){
        table->ring2_id = ring_id;
        table->ring2_hop = test;
    }else if(test > ring2_hop && ring1 == ring2){
        table->ring2_id = ring_id;
        table->ring2_hop = test;
    }

}