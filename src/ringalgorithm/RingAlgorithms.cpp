#include "RingAlgorithms.h"

void RlrecAlgorithms::topology_generate(vector<RingTopologyTuple*>& topology) {

    int noc_size = GlobalParameter::mesh_dim_x;
    int layer;
    int ul, lr;
    for(layer = 1; layer <= noc_size/2; layer++){
        ul = (noc_size*noc_size-2*layer*noc_size+noc_size-2*layer)/2;
        lr = (noc_size*noc_size+2*layer*noc_size-noc_size+2*layer-2)/2;
        topology.push_back(new RingTopologyTuple(ul,lr,1));
        //cout << ul << " " << lr <<endl;
        for(int i = 1;i <= (2*layer-2); i++){
            topology.push_back(new RingTopologyTuple(ul,lr-2*layer+1+i,0));
            //cout << ul << " " << lr-2*layer+1+i <<endl;
            topology.push_back(new RingTopologyTuple(ul+i,lr,0));
            //cout << ul+i << " " << lr <<endl;
        }
        if(layer == 1) continue;
        for(int j = 0;j <= (2*layer-2); j++){
            topology.push_back(new RingTopologyTuple(ul+noc_size*j,lr+(2+j-2*layer)*noc_size,0));
            //cout << ul+noc_size*j << " " << lr+(2+j-2*layer)*noc_size <<endl;
        }
    }
    //layer1下有有两个方向，在这里添加顺时针方向
    topology.push_back(new RingTopologyTuple((noc_size*noc_size-noc_size-2)/2,
                                        (noc_size*noc_size+noc_size)/2,0));
    //cout << (noc_size*noc_size+noc_size)/2 << " " << (noc_size*noc_size-noc_size-2)/2 <<endl;

}

void ImrAlgorithms::topology_generate(vector<RingTopologyTuple *> &topology) {

}
