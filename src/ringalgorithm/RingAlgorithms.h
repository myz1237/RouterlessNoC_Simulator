#ifndef NOCSIM_RINGALGORITHMS_H
#define NOCSIM_RINGALGORITHMS_H
#include "../GlobalParameter.h"

using namespace std;

class RingTopologyTuple{
public:
    int m_ul;
    int m_lr;
    //0--> Clockwise 1-->Anti-clockwise
    int m_dir;
    RingTopologyTuple(int ul,int lr, int dir):m_ul(ul),m_lr(lr),m_dir(dir){}
    ~RingTopologyTuple(){};
};



class RingAlgorithms {
public:
    virtual void topology_generate(vector<RingTopologyTuple*>& topology)=0;
};

class RlrecAlgorithms : public RingAlgorithms {
public:
    void topology_generate(vector<RingTopologyTuple*>& topology);

};
class ImrAlgorithms : public RingAlgorithms{
public:
    void topology_generate(vector<RingTopologyTuple*>& topology);

};
#endif //NOCSIM_RINGALGORITHMS_H
