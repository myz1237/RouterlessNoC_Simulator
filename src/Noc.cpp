
#include "Noc.h"
void Noc::run() {
    initial();

    PLOG_INFO << "Routing Table has been generated...";
    reset_stat();
    if(GlobalParameter::sim_detail){
        //Output the Routing Table of Each Node
        for(int i = 0; i< m_node.size(); i++){
            cout << *m_node[i];
        }
        cout << endl << endl;
        //Output the Statistics of Each Node
        for(int j = 0; j< m_node.size(); j++){
            cout << "Node" << m_node[j]->get_node_id() << ":" << endl;
            m_node[j]->node_info_output();
        }
    }
    //Reset Packet ID
    GlobalParameter::packet_id = 0;

    GlobalParameter::global_cycle = 0;
    //Main Loop of the Simulation
    while(GlobalParameter::global_cycle != GlobalParameter::sim_time){
        PLOG_INFO << "This is Cycle " << GlobalParameter::global_cycle;
#if DEBUG
        packet_tracer();
#endif
        //Move on-ring Packet Forward
        for(int k = 0; k < GlobalParameter::ring.size(); k++){
            GlobalParameter::ring[k]->update_curr_hop();
        }

        for(int q = 0; q < m_node.size(); q++){
            m_node[q]->run(GlobalParameter::global_cycle);
        }

        GlobalParameter::global_cycle++;

        if(GlobalParameter::global_cycle == GlobalParameter::sim_warmup){
            //Warmup 结束 清空统计数据
            reset_stat();
        }

    }
    cout << GlobalParameter::packet_id << endl;
    cout << GlobalParameter::global_cycle << endl;
    //Statistics Output
    stat_gather();
    cout << endl << endl;

/*    for(int q = 0; q < m_node.size(); q++){
        m_node[q]->m_inject->print_packetinfo();
    }*/
}

Noc::Noc() {
    //初始化Node对象
    m_size = GlobalParameter::mesh_dim_x;
    m_node.resize(m_size*m_size);
    for(int i = 0; i < m_size*m_size; i++){
        m_node[i] = new Node(i);
    }
    m_tuple.reserve(cal_ring_num(m_size));
    GlobalParameter::ring.resize(cal_ring_num(m_size));
}

Noc::~Noc() {
    free_vetor<Node*>(m_node);
    delete GlobalParameter::ring_algorithm;
    free_vetor<Ring*>(GlobalParameter::ring);
}

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
        GlobalParameter::ring[i] = new Ring(i, m_tuple[i],m_node);
    }
    //产生实例后清除vector
    free_vetor<RingTopologyTuple*>(m_tuple);
    //Node中的single buffer要在curr_ring_id后初始化
    for(int j = 0; j < m_node.size(); j++){
        m_node[j]->init();
    }
}
//所有node向ring上发送control包 然后等待 不计入cycle中
void Noc::init_routing_table() {
    //向所有ring注入control包
    for(int i = 0; i < m_node.size(); i++){
        m_node[i]->inject_control_packet();
    }

    while(true){
        int counter = 0;

        //Move Packets forward
        for(int j = 0;j< GlobalParameter::ring.size(); j++){
            GlobalParameter::ring[j]->update_curr_hop();
        }


        for(int q = 0; q < m_node.size(); q++){
            m_node[q]->handle_control_packet();
        }

        //single flit检查是否有packet达到 到达后直接送到ejection处理 产生routingtable
        //检查所有ring是否都为空 空了说明routingtable产生完成
        for(int k = 0;k < GlobalParameter::ring.size(); k++){
            //有一个不是空就还没有结束 继续循环
            if(GlobalParameter::ring[k]->is_empty()){
                counter++;
            }
        }
        //所有都为空 跳出循环
        if(counter == GlobalParameter::ring.size()) break;
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
        recv_flit += m_node[i]->get_received_flit();
        recv_packet += m_node[i]->get_received_packet();
        flit_latency += m_node[i]->get_flit_delay();
        packet_latency += m_node[i]->get_packet_delay();
        if(m_node[i]->get_max_flit_delay() > max_flit)
            max_flit = m_node[i]->get_max_flit_delay();
        if(m_node[i]->get_max_packet_delay() > max_packet)
            max_packet = m_node[i]->get_max_packet_delay();
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

void Noc::reset_stat() {
    for(int i =0; i < m_node.size(); i++){
        m_node[i]->reset_stat();
    }
}

int Noc::cal_ring_num(int mesh_x) {
    if(mesh_x == 0 || mesh_x == 1) return 0;
    if(mesh_x == 2) return 2;
    return 1 + 2 * (mesh_x - 2) + mesh_x - 1 + cal_ring_num(mesh_x - 2);
}

void Noc::packet_tracer(){
    for(int j = 0;j< GlobalParameter::ring.size(); j++){
        GlobalParameter::ring[j]->print_packet_info();
    }
}




