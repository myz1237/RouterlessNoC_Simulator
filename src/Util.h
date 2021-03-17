/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Util.h                                                          *
*  @brief    Convenient tools for this program                               *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/
#ifndef NOCSIM_UTIL_H
#define NOCSIM_UTIL_H
#include <vector>
using namespace std;
template <class T>
void free_vetor(vector<T>& t);
template <class T>
void clear_vector(vector<T>& t);

template <class T>
void free_vetor(vector<T>& t){
    //不空的时候返回假 去反为真 释放内存
    if(!t.empty()){
        for(typename vector<T>::iterator it = t.begin(); it != t.end(); it++){
            if(NULL != *it){
                //释放指针指向的对象
                delete *it;
                *it = NULL;
            }
        }
        vector<T>().swap(t);
    }
}

template <class T>
void clear_vector(vector<T>& t){
    if(!t.empty()){
        for(typename vector<T>::iterator it = t.begin(); it != t.end(); it++){
            if(NULL != *it){
                //仅仅清空该内容 不释放
                *it = NULL;
            }
        }
        vector<T>().swap(t);
    }
}

#endif //NOCSIM_UTIL_H
