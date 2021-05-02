#include "ExbManager.h"

int ExbManager::exb_available() {

    for(int i = 0; i < m_exb_status.size(); i++){
        if(!m_exb_status[i]->occupied){
            return i;
        }
    }

    return -1;
}

void ExbManager::set_exb_status(int exb_index, bool status, int buffer_index) {
    m_exb_status[exb_index]->occupied = status;
    m_exb_status[exb_index]->single_buffer_index = buffer_index;
}

void ExbManager::release_exb(int exb_index, int node_id) {
    PLOG_WARNING << "EXB " << exb_index << " is released with single buffer "
                 << m_exb_status[exb_index]->single_buffer_index << " at Node " << node_id << " in Cycle " << GlobalParameter::global_cycle;
    m_exb_status[exb_index]->occupied = false;
    m_exb_status[exb_index]->single_buffer_index = -1;
    m_exb_status[exb_index]->indicator = -1;

}

int ExbManager::check_exb_bound(int single_buffer_index) {
    for(int i = 0;i < m_exb_status.size(); i++){
        //发现这个single buffer已经被绑定了
        if(m_exb_status[i]->single_buffer_index == single_buffer_index){
            return i;
        }
    }
    return -1;
}

int ExbManager::get_exb_remaining_size(int exb_index) const {
    return GlobalParameter::exb_size - m_exb_status[exb_index]->indicator - 1;
}

void ExbManager::push(int exb_index, Flit *flit) {
    m_exb[exb_index][++m_exb_status.at(exb_index)->indicator] = flit;
    flit->set_flit_status(Buffered);
#if DEBUG
    Flit_tracer("Push", flit);
#endif
}

void ExbManager::pop_and_push(int exb_index, Flit *flit, int node_id, bool interrupt) {
    /*Pop out the first flit into the ring*/
    m_exb[exb_index].front()->set_flit_status(Routing);
    if(interrupt)
        PLOG_WARNING << "Pop&Push " << "EXB " << exb_index << " continues interruption " << "at Node "
                     << node_id << " in Cycle " << GlobalParameter::global_cycle << " Forward Packet " <<
                     m_exb[exb_index].front()->get_packet_id() << " Sequence " << m_exb[exb_index].front()->get_sequence();

#if DEBUG
    Flit_tracer("Pop and Push", m_exb[exb_index].front());
#endif
    /*Move forward */
    for(int i = 1; i <= m_exb_status.at(exb_index)->indicator; i++){
        m_exb[exb_index][i-1] = m_exb[exb_index][i];
    }
    /*Insert the new flit*/
    m_exb[exb_index][m_exb_status[exb_index]->indicator] = flit;
    flit->set_flit_status(Buffered);
#if DEBUG
    Flit_tracer("Pop and Push", flit);
#endif
}


void ExbManager::pop(int exb_index, int node_id) {
    m_exb[exb_index].front()->set_flit_status(Routing);
#if DEBUG
    Flit_tracer("Pop ", m_exb[exb_index].front());
#endif
    for(int i = 1; i <= m_exb_status.at(exb_index)->indicator; i++){
        m_exb[exb_index][i-1] = m_exb[exb_index][i];
    }
    m_exb[exb_index][m_exb_status.at(exb_index)->indicator] = nullptr;
    /*Release the Exb if empty after the pop*/
    if(--m_exb_status[exb_index]->indicator == -1){
        PLOG_WARNING << "EXB " << exb_index << " is released with single buffer "
        << m_exb_status[exb_index]->single_buffer_index << " at Node " << node_id << " in Cycle " << GlobalParameter::global_cycle;
        m_exb_status[exb_index]->single_buffer_index = -1;
        m_exb_status[exb_index]->occupied = false;
    }

}

void ExbManager::interrupt_pop(int exb_index, int node_id) {
    m_exb[exb_index].front()->set_flit_status(Routing);
    PLOG_WARNING << "Pop " << "EXB " << exb_index << " continues interruption " << "at Node "
                 << node_id << " in Cycle " << GlobalParameter::global_cycle << " Forward Packet " <<
                 m_exb[exb_index].front()->get_packet_id() << " Sequence " << m_exb[exb_index].front()->get_sequence();
#if DEBUG
    Flit_tracer("Pop ", m_exb[exb_index].front());
#endif
    for(int i = 1; i <= m_exb_status.at(exb_index)->indicator; i++){
        m_exb[exb_index][i-1] = m_exb[exb_index][i];
    }
    m_exb[exb_index][m_exb_status.at(exb_index)->indicator] = nullptr;

    m_exb_status[exb_index]->indicator--;

}

int ExbManager::cal_forward_packet_length(int exb_index, long single_flit_packet_id) {

    int length = m_exb_status[exb_index]->indicator + 1;
    int temp_id = m_exb[exb_index][0]->get_packet_id();

    /*Include the first flit of EXB, so initial value of counter should be 1*/
    int counter = 1;
    for(int i = 1; i < length; i++){
        if(temp_id == m_exb[exb_index][i]->get_packet_id()){
            counter++;
        } else break;
    }
    if(counter == GlobalParameter::exb_size && single_flit_packet_id == temp_id)
        counter++;

    return counter;

}

int ExbManager::cal_forward_packet_length(int exb_index, long single_flit_packet_id, int original_length){
    if(original_length == GlobalParameter::exb_size){
        int temp_id = m_exb[exb_index][0]->get_packet_id();
        if(single_flit_packet_id == temp_id){
            return ++original_length;
        }else{
            return original_length;
        }
    }else{
        return original_length;
    }

}


ExbManager::ExbManager() {
    m_exb.resize(GlobalParameter::exb_num);
    m_exb_status.resize(GlobalParameter::exb_num);
    for(int j = 0; j <GlobalParameter::exb_num; j++){
        m_exb[j].resize(GlobalParameter::exb_size, nullptr);
        m_exb_status[j] = new ExbStatus(false, -1, -1);
    }
}

ExbManager::~ExbManager() {
    vector<vector<Flit*>>().swap(m_exb);
    free_vetor<ExbStatus*>(m_exb_status);
}

void ExbManager::Exb_tracer() {
    for(int i = 0; i<m_exb.size();i++){
        PLOG_INFO << "EXB " << i << "Information";
        for(int j = 0; j<=m_exb_status[i]->indicator;j++){
            PLOG_INFO << "Flit " << j << " ID "<< m_exb[i][j]->m_packet_id <<" Source " << m_exb[i][j]->m_src_id
            << " Dest " << m_exb[i][j]->m_dst_id << " Current " << m_exb[i][j]->m_curr_node << " Sequence " << m_exb[i][j]->m_sequence;
        }
    }
}

void ExbManager::Flit_tracer(const string& str, Flit *flit) {
    PLOG_INFO << str <<"  Flit " << " ID "<< flit->m_packet_id <<" Source " << flit->m_src_id
              << " Dest " << flit->m_dst_id << " Current " << flit->m_curr_node << " Sequence " << flit->m_sequence;
}















