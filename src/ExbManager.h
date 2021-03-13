#ifndef NOCSIM_EXBMANAGER_H
#define NOCSIM_EXBMANAGER_H
#include "Flit.h"
#include "Util.h"
class Flit;



typedef struct ExbStatus{
    //True就是里面有值 被绑定 False就是可用
    bool occupied;
    //也是ring_index
    int single_buffer_index;
    //存储当前EXB内的指向
    int indicator;
    ExbStatus(): occupied(), single_buffer_index(), indicator(){}
    ExbStatus(bool op, int ring_id_index, int indicator):
        occupied(op), single_buffer_index(ring_id_index), indicator(indicator){}
}ExbStatus;

class ExbManager {

public:
    int exb_available();
    //注册EXB使用
    void set_exb_status(int exb_index, bool status, int buffer_index);
    //释放EXB
    void release_exb(int exb_index, int node_id);
    //single_buffer_index和ringid的index相同
    int check_exb_bound(int single_buffer_index);

    int get_exb_remaining_size(int exb_index)const;

    void push(int exb_index, Flit* flit);
    void pop_and_push(int exb_index, Flit* flit);
    //这里加入Node id纯粹为了打印log
    void pop(int exb_index, int node_id);
    ExbManager();
    ~ExbManager();

private:
    vector<vector<Flit*>>m_exb;
    //First存储该exb是否被占用,第二个存储对应single_buffer的index 注意Index而不是 Ring_id
    vector<ExbStatus*>m_exb_status;

};


#endif //NOCSIM_EXBMANAGER_H
