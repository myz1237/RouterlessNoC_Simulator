#include "Node.h"

void Node::init() {
    for (int i = 0; i < m_curr_ring_id.size(); i++){
        m_single_buffer.push_back(nullptr);
    }

}

void Node::recv_flit() {
    for(int i = 0; i<m_curr_ring_id.size(); i++){
        m_single_buffer.at(i) = GlobalParameter::
                ring.at(m_curr_ring_id.at(i))->flit_check(m_node_id);
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
    //把前ej_port_nu 需要注入的ctime设定为-3作区分
    //取二者的小值 防止ej比总的buffer数量大 造成访问不到
    int edge = min<int>(GlobalParameter::ej_port_nu, m_single_buffer.size());
    for(int j = 0; j < edge; j++){
        if(ctime.at(j).second >= 0){
            ctime.at(j).second = -3;
        }
    }
    m_ej_order.swap(ctime);
}

void Node::ej_control_packet() {
    for(int i = 0; i < m_ej_order.size(); i++){
        //只要是到达该node的control 不按照ejectionlink的数目收回而是全部收回
        if(m_ej_order.at(i).second >= 0 || m_ej_order.at(i).second == -3){
            ejection(m_single_buffer.at(m_ej_order.at(i).first),
                     m_curr_ring_id.at(m_ej_order.at(i).first));
        } else if(m_ej_order.at(i).second == -1){
            forward(m_single_buffer.at(m_ej_order.at(i).first));
        }
    }
    reset_single_buffer();
    vector<pair<int,int>>().swap(m_ej_order);
}


void Node::ejection(Flit *flit, int ring_id) {
    //尝试Ejection
    FlitType type = flit->get_flit_type();
    flit->update_hop();
    flit->set_flit_type(Ejected);
    flit->set_atime(GlobalParameter::global_cycle);
    update_flit_stat(flit->calc_flit_latency());

    PLOG_DEBUG_IF(type == Payload) << "Long Packet " << flit->get_packet_id() << " Payload Flit Arrived at Node "
    << m_node_id << " in Cycle " << GlobalParameter::global_cycle << " Sequence No " << flit->get_sequence();

    if (type == Control){
        update_routing_table(flit->get_flit_routing(),m_table, ring_id);
        update_packet_stat(flit->calc_flit_latency());

        //清除该Packet
        GlobalParameter::ring.at(ring_id)->dettach(flit->get_packet_id());
    }else if (type == Header){
        //单个flit的Packet
        //TODO 之后改进的地方 改为sequence判断 sequence为0 说明后面没有flit了
        if(GlobalParameter::ring.at(ring_id)->
        find_packet_length(flit->get_packet_id()) == 1){
            update_packet_stat(flit->calc_flit_latency());

            PLOG_DEBUG << "Single Packet " << flit->get_packet_id() << " Arrived at Node "
            << m_node_id << " in Cycle " << GlobalParameter::global_cycle;

            //清除该packet
            GlobalParameter::ring.at(ring_id)->dettach(flit->get_packet_id());
        }else{
            PLOG_DEBUG << "Long Packet " << flit->get_packet_id() << " Header Flit Arrived at Node "
                                          << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
        }
    }else if (type == Tail){

        PLOG_DEBUG << "Long Packet " << flit->get_packet_id() << " Tail Flit Arrived at Node "
                   << m_node_id << " in Cycle " << GlobalParameter::global_cycle << " Sequence No " << flit->get_sequence();
        update_packet_stat(flit->calc_flit_latency());
        //清除该packet
        GlobalParameter::ring.at(ring_id)->dettach(flit->get_packet_id());
    }
}
void Node::forward(Flit *flit) {
    FlitType type = flit->get_flit_type();
    flit->update_hop();
    if(type == Control){
        //记录当前的node_id和hop_count
        flit->update_routing_snifer();
    }
    //其余需要Forward不用动 等着Ring update就行了
}


Node::Node(int node_id):m_node_id(node_id){
    m_exb_manager = new ExbManager;
    m_inject = new Injection(m_node_id, &m_curr_ring_id);
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


bool Node::comp(pair<int, int> &a, pair<int, int> &b) {
    return a.second > b.second;
}




void Node::handle_rest_flit(int action, int single_flit_index) {
    int exb_index = m_exb_manager->check_exb_binded(single_flit_index);
    if(action == -2){
        if(exb_index != -1){
            m_exb_manager->pop(exb_index, m_node_id);
        }
    }else if(action == -3){
        ejection(m_single_buffer.at(single_flit_index),
                 m_curr_ring_id.at(single_flit_index));
        if(exb_index != -1){
            m_exb_manager->pop(exb_index, m_node_id);
        }
    }else{
        // Action = -1/ >=0
        if(exb_index != -1){
            m_exb_manager->pop_and_push(exb_index,m_single_buffer.at(single_flit_index));
        }else{
            forward(m_single_buffer.at(single_flit_index));
        }
    }
}

bool Node::is_injection_ongoing() {
    if(m_inject->get_ongoing_packet() == nullptr){
        return false;
    }else{
        return true;
    }

}

int Node::inject_eject() {

    //记录这次处理的ring_index 如果注入失败 就返回-1
    int return_ring_index;
    int ring_index;
    int action;
    int selection_stategy = 0;
    int length;
    int exb_index;
    int exb_available_index;
    int remaining_exb_size;
    Packetinfo *p;

    if(!is_injection_ongoing()){
        //此时injection闲
        if(!m_inject->is_packetinfo_empty()){
            //Not Empty
            p = m_inject->get_new_packetinfo();
            length = p->length;
            //选ring 并拿到ring_index
            ring_index = ring_selection(p->dst, selection_stategy);
            action = get_single_buffer_action(ring_index);
            exb_index = m_exb_manager->check_exb_binded(ring_index);
            exb_available_index = m_exb_manager->exb_available();
            //注入成不成功 这条ring的single buffer已经处理过了
            return_ring_index = ring_index;

            //Length判断
            if(length == 1){

                if(action == -2 || action == -3){
                    if(action == -3){
                        //处理该ring上的flit
                        ejection(m_single_buffer.at(ring_index),
                                        m_curr_ring_id.at(ring_index));
                    }
                    if(exb_index == -1){
                        //没有绑定 直接注
                        //直接注入这个长度为1的Packet
                        m_inject->inject_new_packet(ring_index);
                    }else{
                        //还有绑定 需要转发exb内的内容
                        m_exb_manager->pop(exb_index, m_node_id);
                    }

                }else{

                    //不会注入了 查看该ring有没有绑定EXB 处理该选中的ring上的flit
                    //exb_index = m_exb_manager->check_exb_binded(ring_index);
                    if(exb_index != -1){
                        //绑定了
                        //放入EXB中 并弹出EXB第一个flit 此时EXB一定至少一个flit
                        m_exb_manager->pop_and_push(exb_index, m_single_buffer.at(ring_index));
                    }else{
                        //没绑定直接Forward
                        forward(m_single_buffer.at(ring_index));
                    }
                }
            }else{
                //长度大于1

                if(action == -2){

                    if(exb_index != -1){
                        //绑定中
                        //不在注入
                        //TODO 按照论文 这里不做注入 但是action为-3时 同样的情况就在buffer size够下 注入了
                        m_exb_manager->pop(exb_index, m_node_id);
                    }else{
                        //未绑定
                        //有没有可用的exb呢
                        if(exb_available_index != -1){
                            //有可用exb 绑上
                            m_exb_manager->set_exb_status(exb_available_index, true, ring_index);
                            PLOG_WARNING << "EXB " << exb_available_index << " is Bound with single buffer "
                            << ring_index << " at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
                            //产生新的packet
                            m_inject->inject_new_packet(ring_index);
                        }else{
                            //无可用exb No Action
                        }
                    }

                }else if(action == -3){
                    //上来先把buffer里的flit注入了
                    ejection(m_single_buffer.at(ring_index),
                                m_curr_ring_id.at(ring_index));
                    if(exb_index != -1){
                        //绑定中
                        //还有位置吗 记得加上single flit自己的一个size
                        remaining_exb_size = m_exb_manager->get_exb_remaining_size(exb_index) + 1;
                        if(remaining_exb_size >= length){
                            PLOG_WARNING << "EXB " << exb_index << " is Rebound with single buffer "
                                       << ring_index << " at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
                            //还有位置
                            m_inject->inject_new_packet(ring_index);
                        }else{
                            //没位置了 不注入新的packet了 弹出exb中的一个flit
                            m_exb_manager->pop(exb_index, m_node_id);
                        }
                    }else{
                        //未绑定
                        //有没有可用的exb呢
                        if(exb_available_index != -1){
                            //有可用exb 绑上
                            m_exb_manager->set_exb_status(exb_available_index, true, ring_index);
                            PLOG_WARNING << "EXB " << exb_available_index << " is Bound with single buffer "
                                       << ring_index << " at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
                            //产生新的packet
                            m_inject->inject_new_packet(ring_index);
                        }else{
                            //无可用exb No Action
                        }
                    }

                }else{
                    //都是要转发的flit了
                    //不会再注入了
                    //是否被绑定了
                    if(exb_index != -1){
                        m_exb_manager->pop_and_push(exb_index, m_single_buffer.at(ring_index));
                    }else{
                        forward(m_single_buffer.at(ring_index));
                    }
                }

            }

        }else{
            //没有Packetinfo 不需要注入
            return_ring_index = -1;
        }
    }else{
        //true
        //Inject the second flit of the previous packet
        //先拿到之前注入的ring index
        ring_index = m_inject->get_ongoing_ring_index();
        //查看此时的action
        action = get_single_buffer_action(ring_index);
        continue_inject_packet(action);

        return_ring_index = ring_index;
    }
    return return_ring_index;
}

void Node::continue_inject_packet(int action) {
    int flit_index;

    //此时Injection里面一定有未注入完成的Packet
    Packet* p = m_inject->get_ongoing_packet();

    //拿到此时的ring_index 也是single buffer index
    int ring_index = m_inject->get_ongoing_ring_index();

    //拿到此时这个ring上对应的EXB的index
    //TODO 记得检查此处会不会为-1
    //正常来说不会用 因为已经绑定了
    int exb_index = m_exb_manager->check_exb_binded(ring_index);
    PLOG_ERROR_IF(exb_index == -1) << "Error in checking EXB Bound in Continue Injection";
    int remaining_exb_size = m_exb_manager->get_exb_remaining_size(exb_index);
    //查询第一个为Injecting的Flit的位置
    //一定能查到 循环不可能走完的 i最大为长度减一
    for(flit_index = 0; flit_index < p->get_length(); flit_index++){
        if(p->get_flit_status(flit_index) == Injecting){
            break;
        }
    }
    PLOG_ERROR_IF(flit_index == p->get_length()) << "Error in Continue Injection";
    //TODO 记得检查此处的有没有循环走完的情况！！！

    //更改这个flit的状态
    p->set_flit_status(flit_index, Routing);

    if(action == -2 || action == -3){
        //处理可能的注入
        if(action == -3){
            ejection(m_single_buffer.at(ring_index),
                            m_curr_ring_id.at(ring_index));
        }

        if(p->get_flit_type(flit_index) == Tail){
            //注出完成 清空m_ongoing_packet
            m_inject->complete_ongoing_packet();
            PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete Tail Injection at Node " << m_node_id
                       << " in Cycle " << GlobalParameter::global_cycle;
            //检查此时EXB有没有存下来的flit
            if(remaining_exb_size == GlobalParameter::exb_size){
                //还为空 及时释放
                m_exb_manager->release_exb(exb_index, m_node_id);
            }
        }else{
            PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete injection Flit "<< flit_index
                       <<" at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
        }

    }else{
        //处理single buffer的转发
        m_exb_manager->push(exb_index, m_single_buffer.at(ring_index));

        if(p->get_flit_type(flit_index) == Tail){
            //注出完成 清空m_ongoing_packet
            m_inject->complete_ongoing_packet();
            PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete Tail Injection at Node " << m_node_id
                       << " in Cycle " << GlobalParameter::global_cycle;
            //检查此时EXB有没有存下来的flit
        }else{
            PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete injection Flit "<< flit_index
                       <<" at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
        }

    }
}

RoutingTable *Node::check_routing_table(int dst_id) {
    for(int i =0; i < m_table.size(); i++){
        if(dst_id == m_table.at(i)->node_id){
            return m_table.at(i);
        }
    }

}

int Node::ring_selection(int dst, int index) {
    RoutingTable* table = check_routing_table(dst);
    if(index){
        return ring_to_index(table->ring2_id);
    }else{
        return ring_to_index(table->ring1_id);;
    }

}

int Node::ring_to_index(int ring_id) {
    vector<int>::iterator it = find(m_curr_ring_id.begin(), m_curr_ring_id.end(), ring_id);
    //拿到该ringid对应的索引
    return distance(m_curr_ring_id.begin(), it);
}

int Node::get_single_buffer_action(int ring_index) {
    for(int i = 0; i < m_ej_order.size(); i++){
        if(m_ej_order.at(i).first == ring_index){
            return m_ej_order.at(i).second;
        }
    }
}

void Node::run(int cycle) {

    int handled_ring_index;
    int index;
    m_inject->packetinfo_generator(cycle, *GlobalParameter::traffic);
    recv_flit();
    get_ej_order();
    //优先处理要注入的那条ring
    handled_ring_index = inject_eject();
    //处理剩下的single flit
    for(int i = 0; i < m_ej_order.size(); i++){
        index = m_ej_order.at(i).first;
        if(index == handled_ring_index){
            continue;
        }else{
            handle_rest_flit(m_ej_order.at(i).second, index);
        }
    }

    //TODO 记得最后清零所有singleflit 和ej order
    //不要应该也可用做到
    reset_single_buffer();
    vector<pair<int,int>>().swap(m_ej_order);
}

void Node::inject_control_packet() {
    m_inject->controlpacket_generator(0, m_curr_ring_id, GlobalParameter::ring);
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


ostream& operator<<(ostream& out, Node& node){
    int size = node.m_table.size();
    out << "Node" << node.get_node_id() << " " << "Routing Table:"<< endl;
    out << "Number of list: " << size  <<endl;
    for(int j = 0; j < size; j++){
        out << node.m_table.at(j)->node_id
            << "   " << node.m_table.at(j)->ring1_id
            << "   " << node.m_table.at(j)->ring1_hop
            << "   " <<node.m_table.at(j)->ring2_id
            << "   " << node.m_table.at(j)->ring2_hop << endl;
    }
    return out;
}