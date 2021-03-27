#include "Node.h"
void Node::run(long cycle) {
#if DEBUG
    PLOG_INFO << "Node " << m_node_id << " First Print ";
    m_exb_manager->Exb_tracer();
#endif
    int handled_ring_index;
    int index;

    reset_routing_table_index();
    reset_self_loop_counter();

    m_inject->packetinfo_generator(cycle, *GlobalParameter::traffic);
    recv_flit();
    ej_arbitrator();
    handled_ring_index = inject_eject();

    /*Handle the rest of packets*/
    for(int i = 0; i < m_ej_order.size(); i++){
        index = m_ej_order[i].first;
        /*Except the single buffer handled above*/
        if(index == handled_ring_index){
            continue;
        }else{
            handle_rest_flit(m_ej_order[i].second, index);
        }
    }
    /*Clear Input Buffer, wait for next cycle*/
    reset_single_buffer();
    vector<pair<int,int>>().swap(m_ej_order);

#if DEBUG
    PLOG_INFO << "Node " << m_node_id << " Second Print ";
    PLOG_INFO << "Node " << m_node_id << " EXB Tracer";
    m_exb_manager->Exb_tracer();
#endif
}

void Node::sort_routing_table() {
    int size = m_table.size();
    for(int i = 0; i < size; i++){
        m_table[i]->table_sort();
    }
}

void Node::reset_stat() {
    m_stat.reset();
}

void Node::inject_control_packet() {
    m_inject->controlpacket_generator(0, m_curr_ring_id);
}

void Node::handle_control_packet() {

    recv_flit();
    ej_arbitrator();
    ej_fwd_control_packet();
}

int Node::left_injection_queue_packet()const{
    return m_inject->left_packet_in_queue();
}

int Node::left_injection_queue_flit()const{
    return m_inject->left_flit_in_queue();
}

void Node::init() {
    /*Fixed array size */
    m_single_buffer.resize(m_curr_ring_id.size(), nullptr);
}

Node::Node(int node_id):m_node_id(node_id){
    /*Reserve array size with an estimated vale*/
    m_curr_ring_id.reserve(50);
    m_table.reserve(GlobalParameter::mesh_dim_x*GlobalParameter::mesh_dim_x-1);
    m_exb_manager = new ExbManager;
    m_inject = new Injection(m_node_id, &m_curr_ring_id);
    m_stat.reset();
}

Node::~Node() {
    delete m_inject;
    delete m_exb_manager;
    m_exb_manager = nullptr;
    m_inject = nullptr;

    /*Only to clear single buffer values not to delete flit objects*/
    clear_vector<Flit*>(m_single_buffer);
    free_vetor<RoutingTable*>(m_table);
    vector<pair<int, int>>().swap(m_ej_order);
    vector<pair<long, int>>().swap(m_ej_record);
}

void Node::node_info_output() {
    cout << m_stat;
}

void Node::print_routing_table() {
    int size = m_table.size();
    for(int i = 0; i < size; i++){
        RoutingTable* table = m_table[i];
        PLOG_INFO_(1) << "Node: " << m_node_id << " To Node: " << table->node_id;
        int table_size = table->routing.size();
        for(int j = 0;j < table_size; j++){
            PLOG_INFO_(1) << "Available Ring: " << table->routing[j].first
            << " Hop Count: " << table->routing[j].second;
        }
    }
}

void Node::reset_routing_table_index() {
    int size =m_table.size();
    for(int i = 0; i < size; i++){
        m_table[i]->reset_index();
    }
}

void Node::update_flit_stat(int latency) {

    m_stat.received_flit++;
    m_stat.flit_delay += latency;

    if(latency > m_stat.max_flit_delay){
        m_stat.max_flit_delay = latency;
    }
}

void Node::update_packet_stat(int latency, long packet_id) {

    m_stat.received_packet++;
    m_stat.packet_delay += latency;

    if(latency > m_stat.max_packet_delay){
        m_stat.max_packet_delay = latency;
        m_stat.packet_id_for_max_delay = packet_id;
    }
}

void Node::ej_fwd_control_packet() {
    /*First value of pari<int, int> in m_ej_order records index of single buffer or ring across current node*/
    for(int i = 0; i < m_ej_order.size(); i++){
        if(m_ej_order[i].second >= 0 || m_ej_order[i].second == -3){
            ejection(m_single_buffer[m_ej_order[i].first],
                     m_curr_ring_id[m_ej_order[i].first]);
        } else if(m_ej_order[i].second == -1){
            forward(m_single_buffer[m_ej_order[i].first]);
        }
    }

    reset_single_buffer();
    vector<pair<int,int>>().swap(m_ej_order);
}

/*Call ring's function, flit_check, to figure out arrival flits*/
void Node::recv_flit() {
    for(int i = 0; i<m_curr_ring_id.size(); i++){
        m_single_buffer[i] = GlobalParameter::
                ring[m_curr_ring_id[i]]->flit_check(m_node_id);
    }
}

void Node::ejection(Flit *flit, int ring_id) {

    FlitType type = flit->get_flit_type();
    flit->update_hop();
    flit->set_flit_status(Ejected);
    flit->set_atime(GlobalParameter::global_cycle);
    update_flit_stat(flit->calc_flit_latency());

    if (type == Control){
        /*Extract Routing msg from the control Packet and Update the current routing routing*/
        update_routing_table(flit->get_flit_routing(),m_table, ring_id);
        update_packet_stat(flit->calc_flit_latency(),flit->get_packet_id());

        /*Erase and delete this control packet from the ring*/
        GlobalParameter::ring[ring_id]->dettach(flit->get_packet_id());
        return;
    }else if (type == Header){

        if(GlobalParameter::ring[ring_id]->
                find_packet_length_by_ID(flit->get_packet_id()) == 1){
            update_packet_stat(flit->calc_flit_latency(), flit->get_packet_id());

            PLOG_DEBUG << "Single Packet " << flit->get_packet_id() << " Arrived at Node "
                       << m_node_id << " in Cycle " << GlobalParameter::global_cycle;

            /*Erase and delete this packet with one flit from the ring*/
            GlobalParameter::ring[ring_id]->dettach(flit->get_packet_id());
            return;
        }else{
            PLOG_DEBUG << "Long Packet " << flit->get_packet_id() << " Header Flit Arrived at Node "
                       << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
            /*For long packet header, add a new packet record*/
            update_record(flit->get_packet_id(), type);
            return;
        }
    } else if(type == Payload){

        PLOG_DEBUG << "Long Packet " << flit->get_packet_id() << " Payload Flit Arrived at Node "
                   << m_node_id << " in Cycle " << GlobalParameter::global_cycle << " Sequence No " << flit->get_sequence();
        update_record(flit->get_packet_id(), type);
        return;
    } else if (type == Tail){

        PLOG_DEBUG << "Long Packet " << flit->get_packet_id() << " Tail Flit Arrived at Node "
                   << m_node_id << " in Cycle " << GlobalParameter::global_cycle << " Sequence No " << flit->get_sequence();
        update_packet_stat(flit->calc_flit_latency(), flit->get_packet_id());
        update_record(flit->get_packet_id(), type);

        GlobalParameter::ring[ring_id]->dettach(flit->get_packet_id());
    }
}

void Node::forward(Flit *flit) {
    FlitType type = flit->get_flit_type();
    flit->update_hop();
    if(type == Control){
        /*Update routing msg of non-destination control flit*/
        flit->update_routing_snifer();
    } else{
        /* No action for other types
         * Wait for the next cycle
         * */
    }
}

void Node::reset_single_buffer() {
    for (int i = 0; i < m_single_buffer.size(); i++){
        m_single_buffer[i] = nullptr;
    }
}
/* Arbitrator Process:
 *                    Initial ejection order, pari<int, int>.
 *                    Assign single buffer index to the first value, another field is for creation time
 *                    Divide single buffers into two parts, non-dst and in-dst buffers
 *                    Non-dst: Tag the second data field with -1
 *                    In-dst with the packet record:
 *                           Tag header, payload, and tail with 2, 1, 0 respectively
 *                           Record the numbers of each group
 *                    In-dst without the packet record: Tag with -1
 *                    Empty Buffer: Tag -2
 *                    Sort all pairs by the second data field in descending order
 *                    e.g: 2.........2  1.........1  0.........0  -1.........-1  -2.........-2
 *                           Header       Payload       Tail         Non-dst         Null
 *                    Sort three groups by creationg time in descending order seperately
 *                    e.g: 18........3  9.........0  20........5  -1.........-1  -2.........-2
 *                           Header       Payload       Tail         Non-dst         Null
 *                    Reverse it and assign -3 to the first two(Ejection links num) non-negative values
 *                    Except these buffers with the tag -3, others will be forwarded to the next node
 * */
void Node::ej_arbitrator() {
    int tail_index = 0, payload_index = 0, header_index = 0;
    vector<pair<int,int>> ctime(m_single_buffer.size());
    for(int i = 0;i < ctime.size(); i++){
        /*Store index for m_curr_ring_id and m_single_buffer*/
        ctime[i].first = i;
        if(m_single_buffer[i] != nullptr &&
        m_single_buffer[i]->get_flit_dst() == m_node_id){
            //TODO 注意！
            if(m_single_buffer[i]->get_hop() > 255)
                exit(0);
            FlitType type = m_single_buffer[i]->get_flit_type();
            long packet_id = m_single_buffer[i]->get_packet_id();
            int seq = m_single_buffer[i]->get_sequence();
            if(type == Tail){
                if(check_record(packet_id,seq)){
                    ctime[i].second = 0;
                    tail_index++;
                }else{
                    /*No record, forward it*/
                    ctime[i].second = -1;
                }
            }else if(type == Payload){
                if(check_record(packet_id,seq)){
                    ctime[i].second = 1;
                    payload_index++;
                }else{
                    ctime[i].second = -1;
                }
            }else if(type == Control){
                /*Ignore it, for compatibility*/
                ctime[i].second = m_single_buffer[i]->get_flit_ctime();
            }else{
                /*Header*/
                ctime[i].second = 2;
                header_index++;
            }
        } else if(m_single_buffer[i] != nullptr &&
                m_single_buffer[i]->get_flit_dst() != m_node_id){
            /*Non-dst buffer*/
            ctime[i].second = -1;
        }else if(m_single_buffer[i] == nullptr) {
            ctime[i].second = -2;
        }
    }
    sort(ctime.begin(),ctime.end(),comp);
    /*Group Sort*/
    int index;
    for(int j = 0;j < header_index; j++){
        index = ctime[j].first;
        ctime[j].second = m_single_buffer[index]->get_flit_ctime();
    }
    sort(ctime.begin(), ctime.begin()+header_index, comp);
    for(int k = header_index; k < payload_index; k++){
        index = ctime[k].first;
        ctime[k].second = m_single_buffer[index]->get_flit_ctime();
    }
    sort(ctime.begin()+header_index, ctime.begin()+header_index+payload_index, comp);
    for(int q = payload_index; q < tail_index; q++){
        index = ctime[q].first;
        ctime[q].second = m_single_buffer[index]->get_flit_ctime();
    }
    sort(ctime.begin()+header_index+payload_index, ctime.begin()+header_index+payload_index+tail_index, comp);

    reverse(ctime.begin(),ctime.end());

    int edge = min<int>(GlobalParameter::ej_port_nu, m_single_buffer.size());
    index = 0;
    for(int t = 0; t < ctime.size(); t++){
        /*Ejection Tag*/
        if(ctime[t].second >= 0 && index < edge){
            ctime[t].second = -3;
            index++;
            if(index == edge) break;
        }
    }
    m_ej_order.swap(ctime);
    vector<pair<int,int>>().swap(ctime);
}

bool Node::check_record(long packet_id, int seq){
    vector<pair<long, int>>::iterator it = find(m_ej_record.begin(),
                                               m_ej_record.end(),make_pair(packet_id,seq));
    if(it == m_ej_record.end()){
        /*No Record Found*/
        return false;
    }else{
        return true;
    }
}

void Node::update_record(long packet_id, int type){
    int i;
    if(type == Header){
        /*Register a new record and wait for the flit with sequence num 1*/
        m_ej_record.emplace_back(packet_id, 1);
        return;
    }
    for(i = 0; i < m_ej_record.size(); i++){
        if(m_ej_record[i].first == packet_id){
            if(type == Tail){
                m_ej_record.erase(m_ej_record.begin()+i);
                return;
            }
            /*Increase the Expected Sequence Number by 1*/
            m_ej_record[i].second++;
            return;
        }
    }
}

/* Null buffer or buffer to be ejected, check whether this buffer is bound with an EXB
 * If so, pop one flit from the EXB
 * Buffer to be forwarded, if bound, pop one flit and push this buffer into the exb
 * */
void Node::handle_rest_flit(int action, int single_flit_index) {
    int exb_index = m_exb_manager->check_exb_bound(single_flit_index);
    if(action == -2){
        if(exb_index != -1){
            m_exb_manager->pop(exb_index, m_node_id);
        }
    }else if(action == -3){
        ejection(m_single_buffer[single_flit_index],
                 m_curr_ring_id[single_flit_index]);
        if(exb_index != -1){
            m_exb_manager->pop(exb_index, m_node_id);
        }
    }else{
        /*Action = -1 or >=0*/
        if(exb_index != -1){
            m_exb_manager->pop_and_push(exb_index,m_single_buffer[single_flit_index]);
        }else{
            forward(m_single_buffer[single_flit_index]);
        }
    }
}

/*Please check the paper for details*/
int Node::inject_eject(){
    int return_ring_index;
    int ring_index;
    int action;
    int length;
    int exb_index;
    int exb_available_index;
    Packetinfo *p;

    if(!is_injection_ongoing()){

        if(!m_inject->is_packetinfo_empty()){

            /*Retrieve the oldest packetinfo*/
            p = m_inject->get_new_packetinfo();
            length = p->length;

here:       ring_index = ring_selection(p->dst);
            /*Have tries all options, No rings available this cycle*/
            if(ring_index == -1) return -1;

            action = get_single_buffer_action(ring_index);
            exb_index = m_exb_manager->check_exb_bound(ring_index);
            exb_available_index = m_exb_manager->exb_available();
            return_ring_index = ring_index;

            if(action == -2 || action == -3){

                if(exb_index != -1){

                    /*Bound, fail to inject*/
                    /*Try another Ring*/
                    goto here;
                }else{

                    /*No bound*/
                    if(length == 1){
                        /*Inject this single packet*/
                        m_inject->inject_new_packet(ring_index);

                        if(action == -3){
                            /*Do not forget to handle the buffer to be ejected*/
                            ejection(m_single_buffer[ring_index], m_curr_ring_id[ring_index]);
                        }

                    }else{

                        /*Long Packet Injection*/
                        if(exb_available_index != -1){

                            /*EXB Availbale. Bind it and inject the packet*/
                            m_exb_manager->set_exb_status(exb_available_index, true, ring_index);
                            PLOG_WARNING << "EXB " << exb_available_index << " is Bound with single buffer "
                                         << ring_index << " at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
                            /*Inject the header flit of the long packet*/
                            m_inject->inject_new_packet(ring_index);

                            if(action == -3){
                                ejection(m_single_buffer[ring_index], m_curr_ring_id[ring_index]);
                            }

                        }else{
                            /*No action if exb unavailable*/
                            /*Wait for next cycle to inject*/
                            return_ring_index = -1;
                        }
                    }
                }
            }else{

                /*Flit in the input buffer needs to be forwarded*/
                /*Fail to Inject*/
                goto here;
            }

        }else{

            /*Empty injection queue*/
            return_ring_index = -1;
        }

    }else{
        /*Injection is going on, inject the rest flits of the previous packet*/
        continue_inject_packet();
        return_ring_index = m_inject->get_ongoing_ring_index();
    }
    p = nullptr;
    return return_ring_index;
}


bool Node::is_injection_ongoing() {
    if(m_inject->get_ongoing_packet() == nullptr){
        return false;
    }else{
        return true;
    }
}

/*Please check the paper for details*/
void Node::continue_inject_packet() {
    /*Indicate the index of the flit to be injected*/
    int flit_index;

    /*Retrieve the pointer to the previous long packet*/
    Packet* p = m_inject->get_ongoing_packet();

    int ring_index = m_inject->get_ongoing_ring_index();
    int action = get_single_buffer_action(ring_index);

    int exb_index = m_exb_manager->check_exb_bound(ring_index);
    /*PLOG_ERROR_IF(exb_index == -1) << "Error in checking EXB Bound in Continue Injection";*/
    int remaining_exb_size = m_exb_manager->get_exb_remaining_size(exb_index);

    /*Find the first flit to be injected*/
    for(flit_index = 0; flit_index < p->get_length(); ++flit_index){
        if(p->get_flit_status(flit_index) == Injecting){
            break;
        }
    }
    /*PLOG_ERROR_IF(flit_index == p->get_length()) << "Error in Continue Injection"
    << " Packet ID " << p->get_id() << " Size " << p->get_length() << " in Node " << m_node_id
    << " in Cycle " << GlobalParameter::global_cycle;*/

    if(action == -2 || action == -3){

        p->set_flit_status(flit_index, Routing);

        if(action == -3){
            ejection(m_single_buffer[ring_index],
                            m_curr_ring_id[ring_index]);
        }

        /*Act if avg is used*/
        if(GlobalParameter::enable_interrupt&&m_inject->is_injection_interrupted()){
            m_inject->set_interrupt(false);
            PLOG_WARNING << "EXB " << exb_index << " disables interrupt " << " at Node "
            << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
        }

        if(p->get_flit_type(flit_index) == Tail){
            /*Complete this injection after tail flit injection*/
            m_inject->complete_ongoing_packet();
            PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete Tail Injection at Node " << m_node_id
                       << " in Cycle " << GlobalParameter::global_cycle;

            if(remaining_exb_size == GlobalParameter::exb_size){
                /*Release bound EXB if empty*/
                m_exb_manager->release_exb(exb_index, m_node_id);
            }
        }else{
            /*Payload Injeciton Log*/
            PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete injection Flit "<< flit_index
                       <<" at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
        }

    }else{

        if(GlobalParameter::enable_interrupt&&m_inject->is_injection_interrupted()){
            /*Interrupt the injection again*/
            m_inject->set_interrupt(true);

            m_exb_manager->pop_and_push(exb_index, m_single_buffer[ring_index]);

            PLOG_WARNING << "EXB " << exb_index << " enables interrupt again " << " at Node "
                         << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
        }else{
            if(remaining_exb_size == 0){
                /*No space in exb, trigger the interrupt*/
                m_inject->set_interrupt(true);
                m_exb_manager->pop_and_push(exb_index, m_single_buffer[ring_index]);
                /*No injection in this cycle*/
                PLOG_WARNING << "EXB " << exb_index << " enables interrupt " << " at Node "
                << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
            }else{
                /*Normal injection without interrupt*/
                p->set_flit_status(flit_index, Routing);

                m_exb_manager->push(exb_index, m_single_buffer[ring_index]);

                if(p->get_flit_type(flit_index) == Tail){
                    m_inject->complete_ongoing_packet();
                    PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete Tail Injection at Node " << m_node_id
                               << " in Cycle " << GlobalParameter::global_cycle;
                    /*No need to check whether the EXB is empty, because you've pushed one into it*/
                }else{
                    PLOG_DEBUG << "Long Packet " << p->get_id() << " Complete injection Flit "<< flit_index
                               <<" at Node " << m_node_id << " in Cycle " << GlobalParameter::global_cycle;
                }

            }
        }
    }
    p = nullptr;
}

RoutingTable *Node::search_routing_table(int dst_id) {
    int size = m_table.size();
    for(int i =0; i < size; i++){
        if(dst_id == m_table[i]->node_id){
            return m_table[i];
        }
    }
}

int Node::ring_selection(int dst) {
    if(dst == m_node_id){
        int size = m_curr_ring_id.size();
        /*Have tries all options, No rings available this cycle*/
        if(m_self_loop_counter == size) return -1;
        return m_self_loop_counter++;
    }
    RoutingTable* table = search_routing_table(dst);
    int routing_table_size = table->routing.size();
    int &routing_index = table->routing_index;

    /*Have tries all options, No rings available this cycle*/
    if(routing_index == routing_table_size) return -1;

    return ring_to_index(table->routing[routing_index++].first);
}

int Node::ring_to_index(int ring_id) {
    vector<int>::iterator it = find(m_curr_ring_id.begin(), m_curr_ring_id.end(), ring_id);
    /*Get the index of rings across this node*/
    return distance(m_curr_ring_id.begin(), it);
}

int Node::get_single_buffer_action(int ring_index) {
    for(int i = 0; i < m_ej_order.size(); i++){
        if(m_ej_order[i].first == ring_index){
            return m_ej_order[i].second;
        }
    }
}

ostream& operator<<(ostream& out, Stat& stat){
    out << "\t" << "Received Flit: " << stat.received_flit << endl
         << "\t" << "Received Packet: " << stat.received_packet << endl
         << "\t" << "Max Flit Delay: " << stat.max_flit_delay << endl
         << "\t" << "Max Packet Delay: " << stat.max_packet_delay << endl
         << "\t" << "Packet ID of Max Packet Delay: " << stat.packet_id_for_max_delay << endl;
    /*Only used for NoC Statistics, Node Statistics won't enter this statement*/
    if(stat.flit_throughput != 0)
        out  << "\t" << "Flit Throughput: " << stat.flit_throughput << endl
             << "\t" << "Packet Throughput: " << stat.packet_throughput << endl
             << "\t" << "Average Flit Latency: " << stat.avg_flit_latency << endl
             << "\t" << "Average Packet Latency: " << stat.avg_packet_latency << endl;
    return out;
}

bool operator==(const pair<long, int>& a, const pair<long, int>& b){
    return a.first == b.first && a.second == b.second;
}
