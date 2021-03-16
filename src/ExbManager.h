/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     ExbManager.h                                                    *
*  @brief    Extension Buffers are implemented in this file, with two        *
*            main functions, Pop and Push. Our new pipeline buffer           *
*            strategy is also included                                       *
*                                                                            *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_EXBMANAGER_H
#define NOCSIM_EXBMANAGER_H
#include "Flit.h"
#include "Util.h"
class Flit;


/**
 * @brief Status for each extension buffer
 */
typedef struct ExbStatus{

    bool occupied;           /*is occupied or not*/

    int single_buffer_index; /*index for input buffer or ring. Only valid in a Node*/

    int indicator;           /*pointer to the newest flit stored in one exb*/

    ExbStatus(): occupied(), single_buffer_index(), indicator(){}
    ExbStatus(bool op, int ring_id_index, int indicator):
        occupied(op), single_buffer_index(ring_id_index), indicator(indicator){}
}ExbStatus;

/**
 * @brief Extension Buffers array and management
 * @func  Register and release exb
 *        Maintain and Update Exb status.
 *        Provide Pop and Push functions for exb manipulation
 */
class ExbManager {

public:
    /**
    * @return Exact index or -1 if unavailable
    */
    int exb_available();

    /**
    * @brief  Register an Exb
    */
    void set_exb_status(int exb_index, bool status, int buffer_index);

    void release_exb(int exb_index, int node_id);

    /**
    * @brief  Check whether this ring is bound with a exb or not
    * @param  single_buffer_index  Input buffer index in a node
    * @return Exact index or -1 if no binding
    */
    int check_exb_bound(int single_buffer_index);

    int get_exb_remaining_size(int exb_index)const;

    void push(int exb_index, Flit* flit);
    void pop_and_push(int exb_index, Flit* flit);

    /**
    * @brief  Pop one flit from an exb. Release it if empty after the pop action
    * @param  node_id   Just to print a log
    */
    void pop(int exb_index, int node_id);

    ExbManager();
    ~ExbManager();

    void Exb_tracer();
    void Flit_tracer(const string& str, Flit *flit);

private:
    /*Extension Buffer space*/
    vector<vector<Flit*>>m_exb;
    /*Status for each exb
    Use the same index to access one exb and its status*/
    vector<ExbStatus*>m_exb_status;

};


#endif //NOCSIM_EXBMANAGER_H
