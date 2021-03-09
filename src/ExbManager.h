#ifndef NOCSIM_EXBMANAGER_H
#define NOCSIM_EXBMANAGER_H
#include "Flit.h"
#include "Util.h"
class Flit;



typedef struct ExbStatus{
    //TODO 到底要不要这个空闲不空闲 因为他和ring_id_index的状态是一致的 检查ring_id_index就可以知道这个Exb是否再被用
    bool occupied;
    //也是ring_index
    int single_buffer_index;
    //是否释放该EXB
    bool release;
    //存储当前EXB内的指向
    int indicator;
    ExbStatus(): occupied(), single_buffer_index(), release(), indicator(){}
    ExbStatus(bool op, int ring_id_index, bool release, int indicator): occupied(op),
                                                                        single_buffer_index(ring_id_index), release(release), indicator(indicator){}
}ExbStatus;

class ExbManager {

public:
    int exb_available();
    //注册EXB使用
    void set_exb_status(int exb_index, bool status, int buffer_index);
    //通知EXB释放Injection期间的Flit 根据绑定的single buffer的index设定Release信号
    void set_exb_status(int buffer_index, bool release);
    void reset_exb_status(int exb_index);
    //single_buffer_index和ringid的index相同
    int check_exb_binded(int single_buffer_index);
    //True 说明该EXB已经满了 False 还有位置
    bool check_exb_full(int exb_index)const;
    inline bool check_exb_null(int exb_index)const{
        return m_exb_status.at(exb_index)->indicator == -1;
    }
    inline bool check_exb_release(int exb_index)const{
        return m_exb_status.at(exb_index)->release;}

    void push(int exb_index, Flit* flit);
    void pop_and_push(int exb_index, Flit* flit);
    void pop(int exb_index);
    ExbManager();
    ~ExbManager();

private:
    vector<vector<Flit*>>m_exb;
    //First存储该exb是否被占用,第二个存储对应single_buffer的index 注意Index Not Ring_id
    vector<ExbStatus*>m_exb_status;

};


#endif //NOCSIM_EXBMANAGER_H
