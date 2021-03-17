
#include "Noc.h"
void Noc::run() {
    /*Record the number of receive packets during the warm_up stage*/
    long warm_up_packet = 0;

    initial();
    PLOG_INFO << "Routing Table has been generated...";
    reset_stat();


    if(GlobalParameter::sim_detail){
        /*Output the Routing Table of Each Node*/
        for(int i = 0; i< m_node.size(); i++){
            cout << *m_node[i];
        }
        cout << endl << endl;
    }

    /*Clear variables*/
    GlobalParameter::packet_id = 0;
    GlobalParameter::global_cycle = 0;

    /*Main Loop of the Simulation*/
    while(GlobalParameter::global_cycle != GlobalParameter::sim_time){

        PLOG_INFO << "This is Cycle " << GlobalParameter::global_cycle;
#if DEBUG
        packet_tracer();
#endif
        /*Move on-ring Packets Forward*/
        for(int k = 0; k < GlobalParameter::ring.size(); k++){
            GlobalParameter::ring[k]->update_curr_hop();
        }

        /*Receive incoming packets, injection, ejection and buffered*/
        for(int q = 0; q < m_node.size(); q++){
            m_node[q]->run(GlobalParameter::global_cycle);
        }

        /*Warm_up stage ends, clear statistics data*/
        if(GlobalParameter::global_cycle == GlobalParameter::sim_warmup){
            reset_stat();
            warm_up_packet = GlobalParameter::packet_id + 1;
            left_packet_num_gather();
            PLOG_INFO << "Warm Up Ends!";
        }

        GlobalParameter::global_cycle++;
    }
    cout << endl << endl;
    cout << "\t" << "Number of Packet sent during warm up phrase: " << warm_up_packet << endl;
    cout << "\t" << "Number of Packet sent during sim phrase: " << GlobalParameter::packet_id - warm_up_packet + 1<< endl;
    cout << "\t" << "Total Warm up cycle: " << GlobalParameter::sim_warmup << endl;
    cout << "\t" << "Total Sim cycle " << GlobalParameter::sim_time - GlobalParameter::sim_warmup << endl;
    stat_gather();

}

Noc::Noc() {

    m_size = GlobalParameter::mesh_dim_x;
    m_node.resize(m_size*m_size);
    for(int i = 0; i < m_size*m_size; i++){
        m_node[i] = new Node(i);
    }
    m_tuple.reserve(cal_ring_num(m_size));
    GlobalParameter::ring.resize(cal_ring_num(m_size));
    packet_num_correction = 0;

}

Noc::~Noc() {

    free_vetor<Node*>(m_node);
    delete GlobalParameter::ring_algorithm;
    delete GlobalParameter::traffic;
    GlobalParameter::ring_algorithm = nullptr;
    GlobalParameter::traffic = nullptr;
    free_vetor<Ring*>(GlobalParameter::ring);
    /*Output unreceived packets after the simulation stops*/
    cout << "\t" << "UnReceived Packet：" << GlobalParameter::unrecv_packet_num;
}

void Noc::initial() {
    init_ring();
    init_routing_table();
}

void Noc::init_ring() {

    if(GlobalParameter::ring_strategy == IMR){
        GlobalParameter::ring_algorithm = new ImrAlgorithms;
    }else if(GlobalParameter::ring_strategy == RLrec){
        GlobalParameter::ring_algorithm = new RlrecAlgorithms;
    }
    /*Generate results of ring topology*/
    GlobalParameter::ring_algorithm->topology_generate(m_tuple);
    /*New rings through RingTopologyTuple*/
    for(int i = 0;i < m_tuple.size(); i++){
        GlobalParameter::ring[i] = new Ring(i, m_tuple[i],m_node);
    }

    free_vetor<RingTopologyTuple*>(m_tuple);
    for(int j = 0; j < m_node.size(); j++){
        m_node[j]->init();
    }
}

void Noc::init_routing_table() {

    for(int i = 0; i < m_node.size(); i++){
        m_node[i]->inject_control_packet();
    }

    while(true){
        int counter = 0;

        for(int j = 0;j< GlobalParameter::ring.size(); j++){
            GlobalParameter::ring[j]->update_curr_hop();
        }

        for(int q = 0; q < m_node.size(); q++){
            m_node[q]->handle_control_packet();
        }


        for(int k = 0;k < GlobalParameter::ring.size(); k++){
            if(GlobalParameter::ring[k]->is_empty()){
                counter++;
            }
        }
        /*Jump out when there is no on-ring packet in all rings*/
        if(counter == GlobalParameter::ring.size()) break;
    }
}

void Noc::stat_gather() {
    Stat stat;
    int recv_flit = 0;
    int recv_packet = 0;
    int flit_latency = 0;
    int packet_latency = 0;
    int max_flit_delay = 0;
    int max_packet_delay = 0;
    long packet_id_for_max_delay = 0;
    for(int i = 0; i < m_node.size(); i++){
        recv_flit += m_node[i]->get_received_flit();
        recv_packet += m_node[i]->get_received_packet();
        flit_latency += m_node[i]->get_flit_delay();
        packet_latency += m_node[i]->get_packet_delay();
        if(m_node[i]->get_max_flit_delay() > max_flit_delay)
            max_flit_delay = m_node[i]->get_max_flit_delay();
        if(m_node[i]->get_max_packet_delay() > max_packet_delay){
            max_packet_delay = m_node[i]->get_max_packet_delay();
            packet_id_for_max_delay = m_node[i]->get_packet_id_for_max_delay();
        }
    }
    stat.received_flit = recv_flit;
    stat.received_packet = recv_packet - packet_num_correction;
    stat.max_flit_delay = max_flit_delay;
    stat.max_packet_delay = max_packet_delay;
    stat.packet_id_for_max_delay = packet_id_for_max_delay;
    stat.flit_throughput = (double)recv_flit / ( m_size * m_size - 1 ) / ( GlobalParameter::sim_time - GlobalParameter::sim_warmup );
    stat.packet_throughput = (double)recv_packet / ( m_size * m_size - 1 ) / ( GlobalParameter::sim_time - GlobalParameter::sim_warmup );
    stat.avg_flit_latency = (double)flit_latency / recv_flit;
    stat.avg_packet_latency = (double)packet_latency / recv_packet;

    cout << stat;
}

int Noc::left_packet_num_gather() {
    for(int i = 0; i < GlobalParameter::ring.size(); i++){
        packet_num_correction += GlobalParameter::ring[i]->left_packet_num();
    }
    for(int j = 0; j < m_size * m_size; j++){
        packet_num_correction += m_node[j]->left_injecting_packet_num();
    }
    return 0;
}

void Noc::reset_stat() {
    for(int i =0; i < m_node.size(); i++){
        m_node[i]->reset_stat();
    }
}

int Noc::cal_ring_num(int mesh_x)const {
    if(mesh_x == 0 || mesh_x == 1) return 0;
    if(mesh_x == 2) return 2;
    return 1 + 2 * (mesh_x - 2) + mesh_x - 1 + cal_ring_num(mesh_x - 2);
}

void Noc::packet_tracer(){
    for(int j = 0;j< GlobalParameter::ring.size(); j++){
        GlobalParameter::ring[j]->print_packet_info();
    }
}





