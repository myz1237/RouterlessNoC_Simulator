#include "Injection.h"


void Injection::run_injection(const int cycle){
    //TODO 这里使用了多态 要注意
    packetinfo_generator(cycle,*GlobalParameter::traffic);
    injector();
}

inline
int Injection::ring_selection(int dst, int index){
    RoutingTable* table = check_routing_table(dst);
    if(index){
        return ring_to_index(table->ring2_id);
    }else{
        return ring_to_index(table->ring1_id);;
    }

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

void Injection::packet_generator() {
    //队列中有要发送的packetinfor
    if(!m_packetinfo.empty()){
        //TODO 注意这个front返回是该元素的引用--也就是指针的引用 还是指针
        Packet* p = new Packet(GlobalParameter::packet_id,m_local_id,m_packetinfo.front());
        //提交给m_ongoing_packet注入
        m_ongoing_packet = p;
        //删除该packetinfor的对象和清空指针
        delete m_packetinfo.front();
        GlobalParameter::packet_id++;
        m_packetinfo.front() = nullptr;
        m_packetinfo.erase(m_packetinfo.begin());
    }
}

void Injection::controlpacket_generator(const int cycle, vector<int>& curr_ring_id, vector<Ring*>& ring) {
    //这里实际上没有遵守cycle的概念 只是为了方便 同时向所有穿过该node的ring注入control包
    for(int i = 0; i < curr_ring_id.size(); i++){
        Packet* p = new Packet(GlobalParameter::packet_id,1,m_local_id,cycle);
        m_ongoing_packet = p;
        //将该包注册到所有的ring上
        ring.at(curr_ring_id.at(i))->attach(p);
        //不用通知EXB EXB不用动就好 不会有碰撞
        m_ongoing_packet->set_flit_status(0, Routing);
        //packet_id加1
        GlobalParameter::packet_id++;
        //转交控制权
        m_ongoing_packet = nullptr;
        m_injection_status = false;
    }
}

void Injection::injector() {
    if (!m_exb_interrupt.first) {
        if (!m_injection_status) {
            //看看有没有要发送的包
            packet_generator();
            //有包产生了
            if (m_ongoing_packet != nullptr) {
                try_to_inject();
            }
        }else {
            int i;
            //检查当前注入状态 找到最靠前要注入的flit
            for (i = 0; i < m_ongoing_packet->get_length(); i++) {
                if (m_ongoing_packet->get_flit_status(i) == Injecting) {
                    break;
                }
            }//检查之前有没有包因为资源问题还没有开始注入
            if(i == 0){
                try_to_inject();
            }else{
                //说明当前是个长Packet 继续注入就行了
                m_ongoing_packet->set_flit_status(i, Routing);
                //如果是最后一个Flit
                //释放资源
                if (m_ongoing_packet->get_flit_type(i) == Tail) {
                    m_ongoing_packet = nullptr;
                    m_injection_status = false;
                    //TODO 通知EXB释放
                    //通过m_packet_ring_id查找对应的exb 并通知其释放FLit
                    m_exb_manager->set_exb_status(m_injecting_ring_index, true);
                }
            }
        }
    }
}

Injection::Injection(int node_id, vector<int>* ring, vector<RoutingTable*>* table,
                     vector<pair<int, int>>* ej_order, ExbManager* exb, bool status):
                     m_local_id(node_id), m_curr_ring_id(ring), m_table(table),
                     m_ej_order(ej_order), m_exb_manager(exb), m_injection_status(status){
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
    m_table = nullptr;
    m_exb_manager = nullptr;
    m_ej_order = nullptr;
}

RoutingTable* Injection::check_routing_table(int dst_id) {
    int i;
    for(i =0; i < m_table->size(); i++){
        if(dst_id == m_table->at(i)->node_id){
            break;
        }
    }
    return m_table->at(i);
}


int Injection::check_single_buffer_action(int ring_index) {

    for(int i = 0; i < m_ej_order->size(); i++){
        if(m_ej_order->at(i).first == ring_index){
            //如果前ej_port_nu个的ctime均大于等于0 说明这几个是要ejection的 返回特殊值
            //保证之后接收到ctime为正的都是要forward的
            if(m_ej_order->at(i).second >= 0 && i < GlobalParameter::ej_port_nu)
                return -3;
            return m_ej_order->at(i).second;
        }
    }
}

int Injection::ring_to_index(int ring_id){
    vector<int>::iterator it = find(m_curr_ring_id->begin(), m_curr_ring_id->end(), ring_id);
    //拿到该ringid对应的索引
    return distance(m_curr_ring_id->begin(), it);
}

void Injection::try_to_inject(){
    int index = 0;
here:
    int selected_ring_index = ring_selection(m_ongoing_packet->get_dst(), index);
    int action = check_single_buffer_action(selected_ring_index);

    if(action == -2 || action == -3){
        //查看该ring是否已经被绑定了
        if(m_exb_manager->check_exb_binded(selected_ring_index) != -1){
            //Second
            if(GlobalParameter::routing_strategy == Secondwinner){
                index++;
                if(index == 2){
                    m_injection_status = true;
                    return;
                }
                //存一下ring1的值 如果第二次注入仍然失败了m_packet_ring_id还存的是m_packet_ring_id的值
                m_injecting_ring_index = selected_ring_index;
                goto here;
            }else{//未设置尝试second ring 直接Next Cycle
                m_injection_status = true;
                m_injecting_ring_index = selected_ring_index;
                return;
            }
        }else{
            //没有被绑定
            if(m_ongoing_packet->get_length() == 1){
                //直接注入
                GlobalParameter::ring.at(m_curr_ring_id->at(selected_ring_index))
                        ->attach(m_ongoing_packet);
                m_ongoing_packet->set_flit_status(0, Routing);
                m_injection_status = false;
                m_ongoing_packet = nullptr;
                //此时不用在理会m_packet_ring_id的值了 下个cycle如果有新包产生 就覆盖了
                return;
            }else{
                int exb_index = m_exb_manager->exb_available();
                if(exb_index == -1){
                    //EXB不空闲 直接next cycle
                    m_injection_status = true;
                    m_injecting_ring_index = selected_ring_index;
                    return;
                }else{//有空闲的EXB
                    GlobalParameter::ring.at(m_curr_ring_id->at(selected_ring_index))
                            ->attach(m_ongoing_packet);
                    //TODO 设定EXB的状态 注册该EXB
                    m_exb_manager->set_exb_status(exb_index, true, selected_ring_index);
                    //先发个Header
                    m_ongoing_packet->set_flit_status(0, Routing);
                    m_injection_status = true;
                    m_injecting_ring_index = selected_ring_index;
                    return;
                }
            }
        }
    }else{
        if(GlobalParameter::routing_strategy == Secondwinner){
            index++;
            if(index == 2){
                m_injection_status = true;
                return;
            }
            m_injecting_ring_index = selected_ring_index;
            goto here;
        }else{//未设置尝试second ring 直接Next Cycle
            m_injection_status = true;
            m_injecting_ring_index = selected_ring_index;
            return;
        }
    }
}

pair<bool, int> &Injection::get_exb_interrupt(){
    return m_exb_interrupt;
}

void Injection::print_packetinfo() {
    for(int i = 0; i < m_packetinfo.size();i++){
        cout << "Node " << m_local_id << "   " << *m_packetinfo.at(i);
    }
}




