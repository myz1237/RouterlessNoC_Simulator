//
// Created by myz1237 on 2021/2/17.
//

#include "Node.h"

/*void Node::set_curr_ring(int ring_id) {
    m_curr_ring_id.push_back(ring_id);
}*/

void Node::init() {
    for (int i = 0; i < m_curr_ring_id.size(); i++){
        m_single_buffer.push_back(nullptr);
    }
    m_exb_manager = new ExbManager;
    //这时候m_single_buffer, m_curr_ring_id和
    //m_ej_link.reserve(GlobalParameter::ej_port_nu);
}

void Node::recv_flit() {
    for(int i = 0; i<m_curr_ring_id.size(); i++){
        Flit* f = GlobalParameter::ring.at(m_curr_ring_id.at(i))
                ->flit_check(m_node_id);
        if(f == nullptr){
            //没有找到
            continue;
        } else{
            m_single_buffer.at(i) = f;
        }
    }
}

void Node::reset_single_buffer() {
    for (int i = 0; i < m_single_buffer.size(); i++){
        m_single_buffer.at(i) = nullptr;
    }
}

void Node::get_ej_order() {

    vector<pair<int,int>> ctime;
    ctime.resize(m_single_buffer.size());
    for(int i = 0;i < ctime.size(); i++){
        //代表这是index为i的ring上的single flit 用这个index找到对应的ring_id
        ctime.at(i).first = i;
        //在非空的buffer里选择要在这里注入的single buffer
        if(m_single_buffer.at(i) != nullptr &&
        m_single_buffer.at(i)->get_flit_dst() == m_node_id){
            ctime.at(i).second = m_single_buffer.at(i)->get_flit_ctime();
        } else if(m_single_buffer.at(i) != nullptr &&
                m_single_buffer.at(i)->get_flit_dst() != m_node_id){
            //收到了但非目的地就赋值为-1
            ctime.at(i).second = -1;
        }else if(m_single_buffer.at(i) == nullptr) {
            //空
            ctime.at(i).second = -2;
        }
    }
    //从大到小排列
    sort(ctime.begin(),ctime.end(),comp);
    m_ej_order.swap(ctime);
}

//TODO 再考虑一下 所有情况是否都考虑到了
void Node::handle_all_single_buffer() {
    //前GlobalParameter::ej_port_nu个ctime小的 注入
    //注意 前GlobalParameter::ej_port_nu小有可能有-1和-2
    //如果is_ej_full等于ej_port_nu 说明
    int is_ej_full = 0;
    int i = 0;
    for(i = 0; i < m_ej_order.size(); i++){
        int ctime = m_ej_order.at(i).second;
        int ring_set_index = m_ej_order.at(i).first;
        if(ctime == -2){
            //为-2说明后面都为-2 不用再check了
            break;
        }else if(ctime == -1){
            forward(m_single_buffer.at(m_ej_order.at(i).first),
                    m_curr_ring_id.at(ring_set_index));
            continue;
        }else if(ctime >= 0 && is_ej_full < GlobalParameter::ej_port_nu){
            //要在此node注入
            ejection(m_single_buffer.at(m_ej_order.at(i).first),
                     m_curr_ring_id.at(ring_set_index));
            is_ej_full++;
            continue;
        }else if(ctime >= 0){
            //处理应该注入到Node但是因为Ejection Link的限制 只好转发
            forward(m_single_buffer.at(m_ej_order.at(i).first),
                    m_curr_ring_id.at(ring_set_index));
        }
    }
    //全部处理完 清空single buffer&ej_order
    reset_single_buffer();
    vector<pair<int,int>>().swap(m_ej_order);
}

void Node::ej_control_packet() {
    for(int i = 0; i < m_ej_order.size(); i++){
        //只要是到达该node的control 不按照ejectionlink的数目收回而是全部收回
        if(m_ej_order.at(i).second >= 0){
            ejection(m_single_buffer.at(m_ej_order.at(i).first),
                     m_curr_ring_id.at(m_ej_order.at(i).first));
        } else if(m_ej_order.at(i).second == -1){
            forward(m_single_buffer.at(m_ej_order.at(i).first),
                    m_curr_ring_id.at(m_ej_order.at(i).first));
        }
    }
    reset_single_buffer();
    vector<pair<int,int>>().swap(m_ej_order);
}


void Node::ejection(Flit *flit, int ring_id) {
    //尝试Ejection
    FlitType type = flit->get_flit_type();
    flit->update_hop();
    if (type == Control){
        //不计算他的数目和延迟
        flit->set_flit_type(Ejected);
        flit->set_atime(GlobalParameter::global_cycle);
        update_routing_table(flit->get_flit_routing(),m_table, ring_id);
        //清除该Packet
        GlobalParameter::ring.at(ring_id)->dettach(flit->get_packet_id());
    }else if (type == Header){
        flit->set_flit_type(Ejected);
        flit->set_atime(GlobalParameter::global_cycle);
        update_flit_stat(flit->calc_flit_latency());

        //单个flit的Packet
        //TODO 之后改进的地方 改为sequence判断 sequence为0 说明后面没有flit了
        if(GlobalParameter::ring.at(ring_id)->
        find_packet_length(flit->get_packet_id()) == 1){
            update_packet_stat(flit->calc_flit_latency());
            //清除该packet
            GlobalParameter::ring.at(ring_id)->dettach(flit->get_packet_id());
        }
    }else if (type == Payload){
        flit->set_flit_type(Ejected);
        flit->set_atime(GlobalParameter::global_cycle);

        update_flit_stat(flit->calc_flit_latency());
    } else if (type == Tail){
        flit->set_flit_type(Ejected);
        flit->set_atime(GlobalParameter::global_cycle);

        update_flit_stat(flit->calc_flit_latency());
        update_packet_stat(flit->calc_flit_latency());

        //清除该packet
        GlobalParameter::ring.at(ring_id)->dettach(flit->get_packet_id());
    }
}
void Node::forward(Flit *flit, int ring_id) {
    FlitType type = flit->get_flit_type();
    flit->update_hop();
    if(type == Control){
        //记录当前的node_id和hop_count
        flit->update_routing_snifer();
    }else{

    }
}


bool Node::comp(pair<int, int> &a, pair<int, int> &b) {
    return a.second > b.second;
}

Node::Node(int node_id):m_node_id(node_id){
    m_inject = new Injection(m_node_id, &m_curr_ring_id, &m_table,
                             &m_ej_order, m_exb_manager);
    m_stat.reset();
}

Node::~Node() {
    if(m_inject != nullptr){
        delete m_inject;
        m_inject = nullptr;
    }
    if(m_exb_manager != nullptr){
        delete m_exb_manager;
        m_exb_manager = nullptr;
    }
    //注意这里只清空输入buffer的内容 释放空间交给Ring来做
    clear_vector<Flit*>(m_single_buffer);
    free_vetor<RoutingTable*>(m_table);
    vector<pair<int, int>>().swap(m_ej_order);
}

void Node::handle_control_packet() {
    //先从ring上接收包
    recv_flit();
    //检查每个包并记录状态
    get_ej_order();
    //处理包
}

void Node::reset_stat() {
    m_stat.reset();
}


void Node::update_flit_stat(int latency) {
    m_stat.received_flit++;
    m_stat.flit_delay += latency;

    if(latency > m_stat.max_flit_delay){
        m_stat.max_flit_delay = latency;
    }
}


void Node::update_packet_stat(int latency) {
    m_stat.received_packet++;
    m_stat.packet_delay += latency;

    if(latency > m_stat.max_packet_delay){
        m_stat.max_packet_delay = latency;
    }
}

void Node::node_info_output() {
    cout << m_stat;
}


ostream& operator<<(ostream& out, Node& node){
    int size = node.m_table.size();
    out << "Node" << node.get_node_id() << " " << "Routing Table:"<< endl;
    for(int j = 0; j < size; j++){
        out << node.m_table.at(j)->node_id
            << "   " << node.m_table.at(j)->ring1_id
            << "   " << node.m_table.at(j)->ring1_hop
            << "   " <<node.m_table.at(j)->ring2_id
            << "   " << node.m_table.at(j)->ring2_hop << endl;
    }
    return out;
}

ostream& operator<<(ostream& out, Stat& stat){
    out << "\t" << "Received Flit:" << stat.received_flit << endl
         << "\t" << "Received Packet:" << stat.received_packet << endl
         << "\t" << "Max Flit Delay:" << stat.max_flit_delay << endl
         << "\t" << "Max Packet Delay:" << stat.max_packet_delay << endl;
    //不等0说明是NoC调用 因为Node是不计算下面四个指标的 默认为0
    if(stat.flit_throughput != 0)
        out  << "\t" << "Flit Throughput:" << stat.flit_throughput << endl
              << "\t" << "Packet Throughput:" << stat.packet_throughput << endl
              << "\t" << "Average Flit Latency:" << stat.avg_flit_latency << endl
              << "\t" << "Average Packet Latency:" << stat.avg_packet_latency << endl;
    return out;
}


