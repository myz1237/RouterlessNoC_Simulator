
#include "Noc.h"


void Noc::initial() {
    init_ring();
    init_routing_table();
}

//1.根据ring_strategy选择对应的算法类 并调用其generate方法 产生ring的原始结构tuple
//2.通过tuple 产生n个node序列，并给node类上的ringid记录数组添加相应的ringid
void Noc::init_ring() {
    //完成第一步
    if(GlobalParameter::ring_strategy == IMR){
        GlobalParameter::ring_algorithm = new ImrAlgorithms;
    }else if(GlobalParameter::ring_strategy == RLrec){
        GlobalParameter::ring_algorithm = new RlrecAlgorithms;
    }
    GlobalParameter::ring_algorithm->topology_generate(m_tuple);
    //完成第二步 产生ring实例 同时初始化所有node拥有的ring
    for(int i = 0;i < m_tuple.size(); i++){
        GlobalParameter::ring.push_back(new Ring(i, m_tuple.at(i),m_node));
    }
/*    for(int k = 0; k < m_tuple.size(); k++){
        GlobalParameter::ring.at(k)->print_ring_order();
    }*/

    //产生实例后清除vector
    free_vetor<RingTopologyTuple*>(m_tuple);

    //Node中的single buffer要在curr_ring_id后初始化
    for(int j = 0; j < m_node.size(); j++){
        m_node.at(j)->init();
    }
}
//所有node向ring上发送control包 然后等待 不计入cycle中
void Noc::init_routing_table() {
    //向所有ring注入control包
    for(int i = 0; i < m_node.size(); i++){
        m_node.at(i)->m_inject->controlpacket_generator
        (0,m_node.at(i)->m_curr_ring_id,GlobalParameter::ring);
    }

    int flag = 0;
    while(true){
        int counter = 0;
        //Check whether inject or forward in and after the second cycle
        if(flag != 0){
            for(int k = 0; k < m_node.size(); k++){
                m_node.at(k)->ej_control_packet();
            }
        }
        flag++;

        //Move Packets forward
        for(int j = 0;j< GlobalParameter::ring.size(); j++){
            GlobalParameter::ring.at(j)->update_curr_hop();
        }


        for(int q = 0; q < m_node.size(); q++){
            m_node.at(q)->handle_control_packet();
        }

        //single flit检查是否有packet达到 到达后直接送到ejection处理 产生routingtable
        //检查所有ring是否都为空 空了说明routingtable产生完成
        for(int k = 0;k < GlobalParameter::ring.size(); k++){
            //有一个不是空就还没有结束 继续循环
            if(GlobalParameter::ring.at(k)->is_empty()){
                counter++;
            }
        }
        //所有都为空 跳出循环
        if(counter == GlobalParameter::ring.size()) break;
    }
}

void Noc::run() {
    initial();
    cout << "Routing Table has been generated..." << endl;
    reset_stat();
    if(GlobalParameter::sim_detail){
        //Output the Routing Table of Each Node
        for(int i = 0; i< m_node.size(); i++){
            cout << *m_node.at(i);
        }
        cout << endl << endl;
        //Output the Statistics of Each Node
        for(int j = 0; j< m_node.size(); j++){
            cout << "Node" << m_node.at(j)->get_node_id() << ":" << endl;
            m_node.at(j)->node_info_output();
        }
    }
    //Reset Packet ID
    GlobalParameter::packet_id = 0;

    GlobalParameter::global_cycle = 0;
    //Main Loop of the Simulation
    while(GlobalParameter::global_cycle != GlobalParameter::sim_time){
        cout << "This is Cycle " << GlobalParameter::global_cycle << endl;
        //Move on-ring Packet Forward
        for(int k = 0; k < GlobalParameter::ring.size(); k++){
            GlobalParameter::ring.at(k)->update_curr_hop();
        }

        for(int k = 0; k < m_node.size(); k++){
            m_node.at(k)->recv_inj_ej_for(GlobalParameter::global_cycle);
        }

/*        if(GlobalParameter::global_cycle == GlobalParameter::sim_warmup){
            //Warmup 结束 清空统计数据
            reset_stat();
        }*/
        GlobalParameter::global_cycle++;
    }
    cout << GlobalParameter::injection_cycle << endl;
    cout << GlobalParameter::packet_id << endl;
    cout << GlobalParameter::global_cycle << endl;
    //Statistics Output
    stat_gather();
    cout << endl << endl;


/*    for(int q = 0; q < m_node.size(); q++){
        m_node.at(q)->m_inject->print_packetinfo();
    }*/
}

Noc::Noc() {
    //初始化Node对象
    for(int i = 0; i < m_size*m_size; i++){
        m_node.push_back(new Node(i));
    }
    //RingTopology对象在RingAlgorithm中初始化
}

Noc::~Noc() {
    free_vetor<Node*>(m_node);
}

void Noc::reset_stat() {
    for(int i =0; i < m_node.size(); i++){
        m_node.at(i)->reset_stat();
    }
}

void Noc::stat_gather() {
    Stat stat;
    int recv_flit = 0;
    int recv_packet = 0;
    int flit_latency = 0;
    int packet_latency = 0;
    int max_flit = 0;
    int max_packet = 0;
    for(int i = 0; i < m_node.size(); i++){
        recv_flit += m_node.at(i)->get_received_flit();
        recv_packet += m_node.at(i)->get_received_packet();
        flit_latency += m_node.at(i)->get_flit_delay();
        packet_latency += m_node.at(i)->get_packet_delay();
        if(m_node.at(i)->get_max_flit_delay() > max_flit)
            max_flit = m_node.at(i)->get_max_flit_delay();
        if(m_node.at(i)->get_max_packet_delay() > max_packet)
            max_packet = m_node.at(i)->get_max_packet_delay();
    }
    stat.received_flit = recv_flit;
    stat.received_packet = recv_packet;
    stat.max_flit_delay = max_flit;
    stat.max_packet_delay = max_packet;
    stat.flit_throughput = (double)recv_flit / GlobalParameter::global_cycle;
    stat.packet_throughput = (double)recv_packet / GlobalParameter::global_cycle;
    stat.avg_flit_latency = (double)flit_latency / recv_flit;
    stat.avg_packet_latency = (double)packet_latency / recv_packet;

    cout << stat;
}

void Noc::inj_and_ej(int cycle) {

}



