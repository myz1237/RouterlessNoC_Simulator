#include "Injection.h"


void Injection::controlpacket_generator(const int cycle, vector<int>& curr_ring_id) {
    //这里实际上没有遵守cycle的概念 只是为了方便 同时向所有穿过该node的ring注入control包
    Packet* p;
    for(int i = 0; i < curr_ring_id.size(); i++){
        p = new Packet(GlobalParameter::packet_id,1,m_local_id,cycle);
        //将该包注册到所有的ring上
        GlobalParameter::ring[curr_ring_id[i]]->attach(p);
        //不用通知EXB EXB不用动就好 不会有碰撞
        p->set_flit_status(0, Routing);
    }
    p = nullptr;
}

void Injection::inject_new_packet(int ring_index) {
    //注意这个front返回是该元素的引用--也就是指针的引用 还是指针
    Packet* p = new Packet(m_local_id,m_packetinfo.front());
    //删除该packetinfor的对象和清空指针
    GlobalParameter::ring.at(m_curr_ring_id->at(ring_index))->attach(p);

    p->set_flit_status(0, Routing);

    if(p->get_length() != 1){
        //提交给m_ongoing_packet注入
        m_ongoing_packet = p;
        //设置之后要去的ringindex
        m_injecting_ring_index = ring_index;
        PLOG_DEBUG << "Long Packet " << p->get_id() << " with "<< p->get_length()
                   <<" Flit Complete injection Flit 0 at Node " << m_local_id << " Dst " <<  p->get_dst() << " in Cycle " << GlobalParameter::global_cycle;
    }

    delete m_packetinfo.front();
    m_packetinfo.front() = nullptr;
    //TODO 小心这里可能清不掉全部的Packetinfo
    m_packetinfo.erase(m_packetinfo.begin());

    PLOG_DEBUG_IF(p->get_length() == 1) << "Single Packet " << p->get_id()
                                        << " Complete injection at Node " << m_local_id << " Dst " <<  p->get_dst() << " in Cycle " << GlobalParameter::global_cycle;
    p = nullptr;
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

void Injection::packetinfo_generator(int cycle, Traffic& traffic) {
    //第一次
    if(cycle == 0){
        m_ongoing_packetinfo = traffic.traffic_generator(m_local_id, cycle);
    }
    int length = m_ongoing_packetinfo->length;
    int difference = cycle - m_time;
    if(difference * GlobalParameter::injection_rate - length >= 0){
        //改一下时间
        m_ongoing_packetinfo->ctime = cycle;
        if(m_ongoing_packetinfo->dst == m_local_id){
            m_time = cycle;
            m_ongoing_packetinfo = traffic.traffic_generator(m_local_id, cycle);
            return;
        }
        //发送该infor
        packetinfo_attach(m_ongoing_packetinfo, cycle);
        //记录发送时间
        m_time = cycle;
        //再写一个packetinfo进来
        m_ongoing_packetinfo = traffic.traffic_generator(m_local_id, cycle);
    }else{
        //等待下一个cycle
    }
/*
    //到该产生包的周期了
    if(cycle % GlobalParameter::injection_cycle == 0){
        packetinfo_attach(traffic.traffic_generator(m_local_id,cycle),cycle);
    }*/

}

Injection::Injection(int node_id, vector<int>* curr_ring_id):
        m_local_id(node_id), m_curr_ring_id(curr_ring_id){
    m_ongoing_packet = nullptr;
    m_ongoing_packetinfo = nullptr;
    m_time = 0;
    m_injecting_ring_index = -1;
}

Injection::~Injection() {
    PLOG_DEBUG_IF(!m_packetinfo.empty()) << "Node " << m_local_id <<
                                         " Remaining Packet Info " << m_packetinfo.size();
    free_vetor<Packetinfo*>(m_packetinfo);
    m_ongoing_packet = nullptr;
    m_curr_ring_id = nullptr;
    m_ongoing_packetinfo = nullptr;
    m_time = 0;
}

void Injection::packetinfo_attach(Packetinfo *info, int cycle){
    //如果这里检查src和dst相等 就把该packetinfo删除释放 也就是说该节点不会再产生任何packet
/*    if(info->src == info->dst){
        delete info;
        info = nullptr;
        GlobalParameter::packet_id--;
        return;
    }*/
    //src和dst不相等 添加该包信息
    PLOG_INFO << "Packetinfor " <<  info->id <<" Generated at Node " << m_local_id
    << " in Cycle " << cycle;
    m_packetinfo.push_back(info);
}

void Injection::print_packetinfo() {
    for(int i = 0; i < m_packetinfo.size();i++){
        cout << "Node " << m_local_id << "   " << *m_packetinfo[i];
    }
}
