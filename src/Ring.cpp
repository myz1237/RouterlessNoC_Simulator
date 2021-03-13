#include "Ring.h"

void Ring::update_curr_hop() {
    int curr_node;
    for(int i = 0; i<m_packet.size(); i++){
        for(int j = 0; j<m_packet[i]->get_length(); j++){
            //查找在途的flit 并更新
            if(m_packet[i]->get_flit_status(j) == Routing){
                curr_node = m_packet[i]->get_flit_curr_node(j);
                m_packet[i]->set_flit_curr_node(j, find_next_node(curr_node));
            }
        }
    }
}


Flit* Ring::flit_check(int node_id) {
    int i = 0;
    int j = 0;
    int counter = 0;
    for(i = 0; i<m_packet.size(); i++){
        counter++;
        for(j = 0; j<m_packet[i]->get_length(); j++){
            if(m_packet[i]->get_flit_curr_node(j) == node_id &&
            m_packet[i]->get_flit_status(j) == Routing){
                return m_packet[i]->get_flit(j);
            }
        }
    }
    if(counter == m_packet.size()){
        //没有找到
        return nullptr;
    }

}


void Ring::attach(Packet *p) {
    m_packet.push_back(p);
}

void Ring::dettach(int packet_id) {
    for(vector<Packet*>::iterator iter = m_packet.begin(); iter != m_packet.end(); iter++){
        if((*iter)->get_id() == packet_id){
            delete *iter;
            *iter = NULL;
            m_packet.erase(iter);
            break;
        }
    }
}




Ring::~Ring() {
    PLOG_DEBUG_IF(!m_packet.empty()) << "Ring " << m_ring_id <<
    " Remaining Packet  " << m_packet.size();
    free_vetor<Packet*>(m_packet);
    vector<int>().swap(m_ring_node_order);
}


int Ring::find_next_node(int curr_node) {
    vector<int>::iterator it = find(m_ring_node_order.begin(), m_ring_node_order.end(), curr_node);
    if(it == m_ring_node_order.end()){
        cerr << "Cannot find this node" <<endl;
    }
    //说明是最后一个 则返回第一个
    if(it == m_ring_node_order.end()-1){
        return m_ring_node_order.front();
    }
    //否则直接返回下一个
    return *(it+1);
}

int Ring::find_packet_length(int packet_id) {
    for(int i = 0; i < m_packet.size(); i++){
        if(m_packet[i]->get_id() == packet_id){
            return m_packet[i]->get_length();
        }
    }
    return 0;
}

Ring::Ring(int ring_id, RingTopologyTuple *ring_tuple, vector<Node *>& node){
    m_ring_id = ring_id;
    int size = GlobalParameter::mesh_dim_x;
    int length = (ring_tuple->m_lr - ring_tuple->m_ul) % size;
    int width = (ring_tuple->m_lr - ring_tuple->m_ul) / size;
    for(int i = 0; i<=length-1 ; i++){
        m_ring_node_order.push_back(ring_tuple->m_ul+i);
        //把穿过该node的ringid添加到该node的curr_ring数组中
        node[ring_tuple->m_ul+i]->set_curr_ring(ring_id);
    }
    for(int j = 0; j<=width-1; j++){
        m_ring_node_order.push_back(ring_tuple->m_ul+length+j*size);
        node[ring_tuple->m_ul+length+j*size]->set_curr_ring(ring_id);
    }
    for(int k = 0; k<=length; k++){
        m_ring_node_order.push_back(ring_tuple->m_lr-k);
        node[ring_tuple->m_lr-k]->set_curr_ring(ring_id);
    }
    for (int q = width-1; q >= 1; q--) {
        m_ring_node_order.push_back(ring_tuple->m_ul+q*size);
        node[ring_tuple->m_ul+q*size]->set_curr_ring(ring_id);
    }
    //逆时针需要翻转
    if(ring_tuple->m_dir == 1){
        reverse(m_ring_node_order.begin(),m_ring_node_order.end());
    }

}

void Ring::print_ring_order() {
    for(int i=0; i<m_ring_node_order.size();i++){
        cout<< m_ring_node_order.at(i)<<"  ";
    }
    cout << endl;
}

void Ring::print_packet_info(){
    for(int i=0; i<m_packet.size();i++){
        cout<< m_packet.at(i)->get_id()<<"  " << m_packet.at(i)->get_length() << endl;
    }
}







