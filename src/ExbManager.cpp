#include "ExbManager.h"

int ExbManager::exb_available() {
    //找到为空的exb的index并返回
    for(int i = 0; i < m_exb_status.size(); i++){
        if(!m_exb_status.at(i)->occupied){
            return i;
        }
    }
    //没找到 返回-1作为提醒
    return -1;

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
        m_exb_status.push_back(new ExbStatus(false, -1, -1));
    }
}

ExbManager::~ExbManager() {
    for(int i = 0; i < m_exb.size(); i++){
        clear_vector<Flit*>(m_exb.at(i));
    }
    vector<vector<Flit*>>().swap(m_exb);
}


void ExbManager::pop_and_push(int exb_index, Flit *flit) {
    if(get_exb_remaining_size(exb_index) == GlobalParameter::exb_size){
        return;
    }
    //弹出第一个Flit到Ring上
    m_exb.at(exb_index).front()->set_flit_type(Routing);
    //整个exb前进一个单元
    for(int i = 1; i <= m_exb_status.at(exb_index)->indicator; i++){
        m_exb.at(exb_index).at(i-1) = m_exb.at(exb_index).at(i);
    }
    //放入新的flit
    m_exb.at(exb_index).at(m_exb_status.at(exb_index)->indicator) = flit;
    flit->set_flit_type(Buffered);
}

void ExbManager::push(int exb_index, Flit *flit) {
    m_exb.at(exb_index).at(++m_exb_status.at(exb_index)->indicator) = flit;
    flit->set_flit_type(Buffered);
}

void ExbManager::pop(int exb_index) {
    if(get_exb_remaining_size(exb_index) == GlobalParameter::exb_size){
        return;
    }
    //弹出第一个Flit到Ring上
    m_exb.at(exb_index).front()->set_flit_type(Routing);
    //整个exb前进一个单元
    for(int i = 1; i <= m_exb_status.at(exb_index)->indicator; i++){
        m_exb.at(exb_index).at(i-1) = m_exb.at(exb_index).at(i);
    }
    //原来位置置空
    m_exb.at(exb_index).at(m_exb_status.at(exb_index)->indicator) = nullptr;
    //indicator前挪1
    //并判断此时EXB是否空了
    if(--m_exb_status.at(exb_index)->indicator == -1){
        //重置这个EXB的Status
        m_exb_status.at(exb_index)->single_buffer_index = -1;
        m_exb_status.at(exb_index)->occupied = false;
    }

}

int ExbManager::check_exb_binded(int single_buffer_index) {
    for(int i = 0;i < m_exb_status.size(); i++){
        //发现这个single buffer已经被绑定了
        if(m_exb_status.at(i)->single_buffer_index == single_buffer_index){
            return i;
        }
    }
    return -1;
}

void ExbManager::set_exb_status(int exb_index, bool status, int buffer_index) {
    m_exb_status.at(exb_index)->occupied = status;
    //指示对应哪个Ring的index
    m_exb_status.at(exb_index)->single_buffer_index = buffer_index;
}


void ExbManager::reset_exb_status(int exb_index) {
    m_exb_status.at(exb_index)->occupied = false;
    m_exb_status.at(exb_index)->single_buffer_index = -1;
    m_exb_status.at(exb_index)->indicator = -1;
}


bool ExbManager::check_exb_full(int exb_index) const {
    return m_exb_status.at(exb_index)->indicator == GlobalParameter::exb_size - 1;
}

int ExbManager::get_exb_remaining_size(int exb_index) const {
    return GlobalParameter::exb_size - m_exb_status.at(exb_index)->indicator - 1;
}





