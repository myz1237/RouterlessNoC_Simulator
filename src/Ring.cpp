#include "Ring.h"

int Ring::left_flit() {
    int left_flit = 0;
    for(int i = 0; i<m_packet.size(); i++){
        for(int j = 0; j<m_packet[i]->get_length(); j++){
            if(m_packet[i]->get_flit_status(j) != Ejected)
                left_flit++;
        }
    }
    return left_flit;
}

/*Loop to find Flits with 'Routing' status and assign these flits to the next node id*/
void Ring::update_curr_hop() {
    int curr_node;
    for(int i = 0; i<m_packet.size(); i++){
        for(int j = 0; j<m_packet[i]->get_length(); j++){
            if(m_packet[i]->get_flit_status(j) == Routing){
                curr_node = m_packet[i]->get_flit_curr_node(j);
                /*Set the next node id*/
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
        /*No flit arrives in this node*/
        return nullptr;
    }

}

void Ring::dettach(long packet_id) {
    for(vector<Packet*>::iterator iter = m_packet.begin(); iter != m_packet.end(); iter++){
        if((*iter)->get_id() == packet_id){
            delete *iter;
            *iter = NULL;
            m_packet.erase(iter);
            break;
        }
    }
}

int Ring::find_packet_length_by_ID(long packet_id) {
    for(int i = 0; i < m_packet.size(); i++){
        if(m_packet[i]->get_id() == packet_id){
            return m_packet[i]->get_length();
        }
    }
    return 0;
}

/*Translate ring_tuple into several node ids*/
Ring::Ring(int ring_id, RingTopologyTuple *ring_tuple, vector<Node *>& node){
    m_ring_id = ring_id;
    int size = GlobalParameter::mesh_dim_x;
    int length = (ring_tuple->m_lr - ring_tuple->m_ul) % size;
    int width = (ring_tuple->m_lr - ring_tuple->m_ul) / size;
    for(int i = 0; i<=length-1 ; i++){
        m_ring_node_order.push_back(ring_tuple->m_ul+i);

        /*Record this ring id to the node's vector, m_curr_ring_id*/
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
    /*Reverse needed, if anti-clockwise*/
    if(ring_tuple->m_dir == 1){
        reverse(m_ring_node_order.begin(),m_ring_node_order.end());
    }

}

Ring::~Ring() {
#if DEBUG
    PLOG_DEBUG_IF(!m_packet.empty()) << "Ring " << m_ring_id <<
    " Remaining Packet  " << m_packet.size();
#endif
    /*Add on-ring packets after simulation ends to the variable */
    GlobalParameter::unrecv_packet_num += m_packet.size();
    free_vetor<Packet*>(m_packet);
    vector<int>().swap(m_ring_node_order);
}

int Ring::find_next_node(int curr_node) {
    vector<int>::iterator it = find(m_ring_node_order.begin(), m_ring_node_order.end(), curr_node);
    if(it == m_ring_node_order.end()){
        cerr << "Cannot find this node" <<endl;
    }
    /*End of the vector, return the first element*/
    if(it == m_ring_node_order.end()-1){
        return m_ring_node_order.front();
    }
    /*Return the next element*/
    return *(it+1);
}

void Ring::print_node_order_on_ring() {
    PLOG_INFO_(1) << "Ring ID " << m_ring_id;
    for(int i=0; i<m_ring_node_order.size();i++){

        PLOG_INFO_(1) << " Node on the ring: " <<m_ring_node_order.at(i);
    }
    cout << endl;
}

void Ring::print_onring_packet_flit_info(){
    for(int i=0; i<m_packet.size();i++){
        PLOG_INFO<< "Packet ID " << m_packet[i]->get_id() << " Sourece "
                 << m_packet[i]->get_src()<< " Dest "<< m_packet[i]->get_dst();
        for(int j = 0; j<m_packet[i]->get_length();j++){
            PLOG_INFO << "Flit" << j << " Current Node " << m_packet[i]->get_flit_curr_node(j)
            << " Status " << m_packet[i]->get_flit_status(j);
        }

    }
}

void Ring::print_packet_info(long packet_id) {
    for (int i = 0; i < m_packet.size(); i++) {
        if (m_packet[i]->get_id() == packet_id) {
            PLOG_INFO_(1) << "Packet ID " << m_packet[i]->get_id() << " Sourece "
                          << m_packet[i]->get_src() << " Dest " << m_packet[i]->get_dst()
                          << " Ring ID: " << m_ring_id;
        }
    }
}







