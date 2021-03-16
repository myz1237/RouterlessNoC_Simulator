#include "Injection.h"


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

    Packet* p = new Packet(m_local_id,m_packetinfo.front());

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

    delete m_packetinfo.front();
    m_packetinfo.front() = nullptr;
    /*Injection Complete, delete the first element and move forward the queue*/
    m_packetinfo.erase(m_packetinfo.begin());

    PLOG_DEBUG_IF(p->get_length() == 1) << "Single Packet " << p->get_id()
                                        << " Complete injection at Node " << m_local_id << " Dst " <<  p->get_dst() << " in Cycle " << GlobalParameter::global_cycle;
    p = nullptr;
}

void Injection::packetinfo_generator(int cycle, Traffic& traffic) {
    /*First Cycle, the entrance of the injection loop*/
    if(cycle == 0){
        m_ongoing_packetinfo = traffic.traffic_generator(m_local_id, cycle);
    }
    int length = m_ongoing_packetinfo->length;
    int difference = cycle - m_time;
    if(difference * GlobalParameter::injection_rate - length >= 0){
        m_ongoing_packetinfo->ctime = cycle;
        /*Abandon non-meaning packetinfo and get a new one*/
        if(m_ongoing_packetinfo->dst == m_local_id){
            m_time = cycle;
            m_ongoing_packetinfo = traffic.traffic_generator(m_local_id, cycle);
            return;
        }

        packetinfo_attach(m_ongoing_packetinfo, cycle);
        /*Record this injection cycle*/
        m_time = cycle;
        /*Get a new packetinfo for next cycle*/
        m_ongoing_packetinfo = traffic.traffic_generator(m_local_id, cycle);
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
    PLOG_DEBUG_IF(!m_packetinfo.empty()) << "Node " << m_local_id <<
                                         " Remaining Packet Info " << m_packetinfo.size();
#endif
    GlobalParameter::unrecv_packet_num += m_packetinfo.size();
    free_vetor<Packetinfo*>(m_packetinfo);
    m_ongoing_packet = nullptr;
    m_curr_ring_id = nullptr;
    m_ongoing_packetinfo = nullptr;
    m_time = 0;
}

void Injection::print_packetinfo() {
    for(int i = 0; i < m_packetinfo.size();i++){
        cout << "Node " << m_local_id << "   " << *m_packetinfo[i];
    }
}

void Injection::packetinfo_attach(Packetinfo *info, int cycle){
    //src和dst不相等 添加该包信息
    PLOG_INFO << "Packetinfor " <<  info->id <<" Generated at Node " << m_local_id
              << " in Cycle " << cycle;
    m_packetinfo.push_back(info);
}



