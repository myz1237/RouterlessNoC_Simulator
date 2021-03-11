#include "Injection.h"


void Injection::run_injection(const int cycle){
    //TODO 这里使用了多态 要注意
    packetinfo_generator(cycle,*GlobalParameter::traffic);
}


void Injection::packetinfo_attach(Packetinfo *info){
    //如果这里检查src和dst相等 就把该packetinfo删除释放 也就是说该节点不会再产生任何packet
    if(info->src == info->dst){
        delete info;
        info = nullptr;
        return;
    }
    //src和dst不相等 添加该包信息
    m_packetinfo.push_back(info);
}


void Injection::packetinfo_generator(int cycle, Traffic& traffic) {
    //到该产生包的周期了
    if(cycle % GlobalParameter::injection_cycle == 0){
        packetinfo_attach(traffic.traffic_generator(m_local_id,cycle));

    }

}


void Injection::controlpacket_generator(const int cycle, vector<int>& curr_ring_id, vector<Ring*>& ring) {
    //这里实际上没有遵守cycle的概念 只是为了方便 同时向所有穿过该node的ring注入control包
    for(int i = 0; i < curr_ring_id.size(); i++){
        Packet* p = new Packet(GlobalParameter::packet_id,1,m_local_id,cycle);
        //将该包注册到所有的ring上
        ring.at(curr_ring_id.at(i))->attach(p);
        //不用通知EXB EXB不用动就好 不会有碰撞
        p->set_flit_status(0, Routing);
        //packet_id加1
        GlobalParameter::packet_id++;
    }
}

Injection::Injection(int node_id, vector<int>* ring, bool status):
                     m_local_id(node_id), m_curr_ring_id(ring), m_injection_status(status){
    m_ongoing_packet = nullptr;
    m_exb_interrupt.first = false;
    m_exb_interrupt.second = -1;
    m_injecting_ring_index = -1;
}

Injection::~Injection() {
    free_vetor<Packetinfo*>(m_packetinfo);
    pair<bool, int>().swap(m_exb_interrupt);
    m_ongoing_packet = nullptr;
    m_curr_ring_id = nullptr;

}



pair<bool, int> &Injection::get_exb_interrupt(){
    return m_exb_interrupt;
}

void Injection::print_packetinfo() {
    for(int i = 0; i < m_packetinfo.size();i++){
        cout << "Node " << m_local_id << "   " << *m_packetinfo.at(i);
    }
}

void Injection::inject_new_packet(int ring_index) {
    //注意这个front返回是该元素的引用--也就是指针的引用 还是指针
    Packet* p = new Packet(GlobalParameter::packet_id,m_local_id,m_packetinfo.front());
    //删除该packetinfor的对象和清空指针

    GlobalParameter::ring.at(m_curr_ring_id->at(ring_index))->attach(p);
    p->set_flit_status(0, Routing);

    if(p->get_length() != 1){
        //提交给m_ongoing_packet注入
        m_ongoing_packet = p;
        //设置之后要去的ringindex
        m_injecting_ring_index = ring_index;
    }

    delete m_packetinfo.front();
    m_packetinfo.front() = nullptr;
    //TODO 小心这里可能清不掉全部的Packetinfo
    m_packetinfo.erase(m_packetinfo.begin());
}

bool Injection::is_packetinfo_empty() {
    //empty返回1 否则0
    if(m_packetinfo.empty()){
        return true;
    } else{
        return false;
    }
}

Packetinfo *Injection::get_new_packetinfo() {
    return m_packetinfo.front();
}





