#include "Injection.h"

int Injection::left_flit_in_queue() {
    int left_flit = 0;
    for(int i = 0; i < m_injection_queue.size(); i++){
        left_flit += m_injection_queue[i]->length;
    }
    return left_flit;
}

void Injection::controlpacket_generator(const int cycle, vector<int>& curr_ring_id) {
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

void Injection::packetinfo_generator(int cycle, Traffic& traffic) {
    /*First Cycle, the entrance of the injection loop*/
    if(cycle == 0){
        m_ongoing_packetinfo = traffic.traffic_generator(m_local_id);
    }
    int length = m_ongoing_packetinfo->length;
    int difference = cycle - m_time;
    if(difference * GlobalParameter::injection_rate - length >= 0){
        /*Abandon non-meaning packetinfo and get a new one*/
        if(m_ongoing_packetinfo->dst == m_local_id){
            //m_time = cycle;
            GlobalParameter::packet_id--;
            m_ongoing_packetinfo = traffic.traffic_generator(m_local_id);
            return;
        }
        m_ongoing_packetinfo->ctime = cycle;
        packetinfo_attach(m_ongoing_packetinfo, cycle);
        /*Record this injection cycle*/
        m_time = cycle;
        /*Get a new packetinfo for next cycle*/
        m_ongoing_packetinfo = traffic.traffic_generator(m_local_id);
    }else{
        /*No action, wait for the next cycle*/
    }
}

Injection::Injection(int node_id, vector<int>* curr_ring_id):
        m_local_id(node_id), m_curr_ring_id(curr_ring_id){
    m_ongoing_packet = nullptr;
    m_ongoing_packetinfo = nullptr;
    m_interrupt = false;
    m_time = 0;
    m_injecting_ring_index = -1;
}

Injection::~Injection() {
#if DEBUG
    PLOG_DEBUG_IF(!m_injection_queue.empty()) << "Node " << m_local_id <<
                                         " Remaining Packet Info " << m_injection_queue.size();
#endif
    GlobalParameter::unrecv_packet_num += m_injection_queue.size();
    free_vetor<Packetinfo*>(m_injection_queue);
    if(m_ongoing_packetinfo != nullptr)
        delete m_ongoing_packetinfo;
    m_ongoing_packet = nullptr;
    m_curr_ring_id = nullptr;
    m_ongoing_packetinfo = nullptr;
}

void Injection::print_packetinfo() {
    for(int i = 0; i < m_injection_queue.size(); i++){
        cout << "Node " << m_local_id << "   " << *m_injection_queue[i];
    }
}

void Injection::packetinfo_attach(Packetinfo *info, int cycle){
    //src和dst不相等 添加该包信息
    PLOG_INFO << "Packetinfor " <<  info->id <<" Generated at Node " << m_local_id
              << " in Cycle " << cycle;
    m_injection_queue.push_back(info);
}





