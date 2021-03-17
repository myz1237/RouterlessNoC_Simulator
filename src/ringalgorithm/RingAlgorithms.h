#ifndef NOCSIM_RINGALGORITHMS_H
#define NOCSIM_RINGALGORITHMS_H
#include "../GlobalParameter.h"

using namespace std;
/**
 * @brief Class RingTopologyTuple Definition
 *        Use a simple way to store one ring
 *        e.g: 0 1 2
 *             3   5 with Clockwise ===> (0, 8, 0)
 *             6 7 8
 */
class RingTopologyTuple{
public:
    int m_ul;  /*Id of node in the Upper left corner*/
    int m_lr;  /*Id of node in the lower right corner*/
    int m_dir; /*0--> Clockwise 1-->Anti-clockwise*/
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
/*Less efficient Algorithm, under Construction*/
class ImrAlgorithms : public RingAlgorithms{
public:
    void topology_generate(vector<RingTopologyTuple*>& topology);

};
#endif //NOCSIM_RINGALGORITHMS_H
