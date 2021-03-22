#include "Injection.h"

int Injection::left_flit_in_queue() {
    int left_flit = 0;
    for(int i = 0; i < m_injection_queue.size(); i++){
        left_flit += m_injection_queue[i]->length;
    }
    return left_flit;
}

void Injection::controlpacket_generator(const long cycle, vector<int>& curr_ring_id) {
    Packet* p;
    /*Loop to access all id of rings across this node*/
    for(int i = 0; i < curr_ring_id.size(); i++){
        /*Call Control packet constructor*/
        p = new Packet(GlobalParameter::packet_id,1,m_local_id,cycle);
        /*Register this packet on the ring*/
        GlobalParameter::ring[curr_ring_id[i]]->attach(p);
        /*Change status, move to next node*/
        p->set_flit_status(0, Routing);
    }
    p = nullptr;
}

void Injection::inject_new_packet(int ring_index) {

    Packet* p = new Packet(m_local_id, m_injection_queue.front());

    GlobalParameter::ring.at(m_curr_ring_id->at(ring_index))->attach(p);

    p->set_flit_status(0, Routing);

    if(p->get_length() != 1){
        /*Store the pointer of this long packet*/
        m_ongoing_packet = p;
        /*Record which ring_index it used*/
        m_injecting_ring_index = ring_index;
        PLOG_DEBUG << "Long Packet " << p->get_id() << " with "<< p->get_length()
                   <<" Flit Complete injection Flit 0 at Node " << m_local_id << " Dst " <<  p->get_dst() << " in Cycle " << GlobalParameter::global_cycle;
    }

    delete m_injection_queue.front();
    m_injection_queue.front() = nullptr;
    /*Injection Complete, delete the first element and move forward the queue*/
    m_injection_queue.erase(m_injection_queue.begin());

    PLOG_DEBUG_IF(p->get_length() == 1) << "Single Packet " << p->get_id()
                                        << " Complete injection at Node " << m_local_id << " Dst " <<  p->get_dst() << " in Cycle " << GlobalParameter::global_cycle;
    p = nullptr;
}

void Injection::packetinfo_generator(const long cycle, Traffic& traffic) {

    Packetinfo* p = traffic.traffic_generator(m_local_id);
    if(p != nullptr){

        p->ctime = cycle;
        packetinfo_attach(p, cycle);
    }
}

Injection::Injection(int node_id, vector<int>* curr_ring_id):
        m_local_id(node_id), m_curr_ring_id(curr_ring_id){
    m_ongoing_packet = nullptr;
    m_interrupt = false;
    m_injecting_ring_index = -1;
}

Injection::~Injection() {
#if DEBUG
    PLOG_DEBUG_IF(!m_injection_queue.empty()) << "Node " << m_local_id <<
                                         " Remaining Packet Info " << m_injection_queue.size();
#endif
    GlobalParameter::unrecv_packet_num += m_injection_queue.size();
    free_vetor<Packetinfo*>(m_injection_queue);
    m_ongoing_packet = nullptr;
    m_curr_ring_id = nullptr;
}

void Injection::print_packetinfo() {
    for(int i = 0; i < m_injection_queue.size(); i++){
        cout << "Node " << m_local_id << "   " << *m_injection_queue[i];
    }
}

void Injection::packetinfo_attach(Packetinfo *info, const long cycle){
    PLOG_INFO << "Packetinfor " <<  info->id <<" Generated at Node " << m_local_id
              << " in Cycle " << cycle;
    m_injection_queue.push_back(info);
}





