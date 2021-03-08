#include "ExbManager.h"

int ExbManager::exb_available() {
    int i;
    //找到为空的exb的index并返回
    for(i = 0; i < m_exb_status.size(); i++){
        if(!m_exb_status.at(i)->occupied){
            break;
        }
    }
    //没找到 返回-1作为提醒
    if(i == m_exb_status.size()){
        return -1;
    }else return i;
}

ExbManager::ExbManager() {
    m_exb.reserve(GlobalParameter::exb_num);
    m_exb_status.reserve(GlobalParameter::exb_num);
    for(int j = 0; j <GlobalParameter::exb_num; j++){
        vector<Flit*>temp;
        temp.reserve(GlobalParameter::exb_size);
        for(int k = 0; k < GlobalParameter::exb_size; k++){
            temp.push_back(nullptr);
        }
        m_exb.push_back(temp);
        m_exb_status.push_back(new ExbStatus(false, -1, false));
    }
}

ExbManager::~ExbManager() {
    for(int i = 0; i < m_exb.size(); i++){
        clear_vector<Flit*>(m_exb.at(i));
    }
    vector<vector<Flit*>>().swap(m_exb);
}


void ExbManager::pop_and_push(int index, Flit *flit) {
    //弹出第一个Flit到Ring上
    m_exb.at(index).front()->set_flit_type(Routing);
    //整个exb前进一个单元
    for(int i = 1; i <= m_exb_status.at(index)->indicator; i++){
        m_exb.at(index).at(i-1) = m_exb.at(index).at(i);
    }
    //放入新的flit
    m_exb.at(index).at(m_exb_status.at(index)->indicator) = flit;


}

void ExbManager::push(int index, Flit *flit) {
    m_exb.at(index).at(++m_exb_status.at(index)->indicator) = flit;
}

void ExbManager::pop(int index) {
    //弹出第一个Flit到Ring上
    m_exb.at(index).front()->set_flit_type(Routing);
    //整个exb前进一个单元
    for(int i = 1; i <= m_exb_status.at(index)->indicator; i++){
        m_exb.at(index).at(i-1) = m_exb.at(index).at(i);
    }
    //原来位置置空
    m_exb.at(index).at(m_exb_status.at(index)->indicator) = nullptr;
    //indicator前挪1
    //并判断是否EXB为空
    if(--m_exb_status.at(index)->indicator == -1){
        //重置这个EXB的Status
        m_exb_status.at(index)->ring_id_index = -1;
        m_exb_status.at(index)->occupied = false;
        m_exb_status.at(index)->release = false;
    }

}



void ExbManager::set_exb_status(int index, bool status, int buffer_index) {
    m_exb_status.at(index)->occupied = status;
    //指示对应哪个Ring的index
    m_exb_status.at(index)->ring_id_index = buffer_index;
    m_exb_status.at(index)->release = false;
}

int ExbManager::check_exb_binded(int single_buffer_index) {
    for(int i = 0;i < m_exb_status.size(); i++){
        //发现这个single buffer已经被绑定了
        if(m_exb_status.at(i)->ring_id_index == single_buffer_index){
            return i;
        }
    }
    return -1;
}

void ExbManager::set_exb_status(int buffer_index, bool release) {
    for(int i = 0;i < m_exb_status.size(); i++){
        if(m_exb_status.at(i)->ring_id_index == buffer_index){
            m_exb_status.at(i)->release = release;
        }
    }
}

bool ExbManager::check_exb_full(int single_buffer_index) {
    int exb_index = check_exb_binded(single_buffer_index);

    if(exb_index == -1){
        cerr << "Error in Exb" << endl;
    }else{
        return m_exb_status.at(exb_index)->indicator == (GlobalParameter::exb_size -1);
    }
    return false;
}


