#include "RingAlgorithms.h"

void RlrecAlgorithms::topology_generate(vector<RingTopologyTuple*>& topology) {

    int noc_size = GlobalParameter::mesh_dim_x;
    int layer;
    int ul, lr;
    bool rotate;
    for(layer = 1; layer <= noc_size/2; layer++){
        ul = (noc_size*noc_size-2*layer*noc_size+noc_size-2*layer)/2;
        lr = (noc_size*noc_size+2*layer*noc_size-noc_size+2*layer-2)/2;
        /*No need to rotate*/
        /*Group A*/
        topology.push_back(new RingTopologyTuple(ul,lr,1));
        //cout << ul << " " << lr <<endl;
        rotate = ((noc_size / 2 - layer) % 2) == 1;
        for(int i = 1;i <= (2*layer-2); i++){
            if(!rotate){
                /*Group B*/
                topology.push_back(new RingTopologyTuple(ul,lr-2*layer+1+i,0));
                //cout << ul << " " << lr-2*layer+1+i <<endl;
                /*Group C*/
                topology.push_back(new RingTopologyTuple(ul+i,lr,0));
                //cout << ul+i << " " << lr <<endl;
            }else{

                /*Rotate 90 degrees*/
                topology.push_back(new RingTopologyTuple(ul,lr+(1+i-2*layer)*noc_size,0));

                topology.push_back(new RingTopologyTuple(ul+noc_size*i,lr,0));
            }
        }

        if(layer == 1){
            /*No need to rotate*/
            topology.push_back(new RingTopologyTuple(ul,lr,0));
            continue;
        }

        for(int j = 0;j <= (2*layer-2); j++){
            /*Group D*/
            if(!rotate){
                topology.push_back(new RingTopologyTuple(ul+noc_size*j,lr+(2+j-2*layer)*noc_size,0));
                //cout << ul+noc_size*j << " " << lr+(2+j-2*layer)*noc_size <<endl;
            }else{
                /*Rotate 90 degrees*/
                topology.push_back(new RingTopologyTuple(ul+j,lr-2*layer+2+j,0));
            }

        }
    }

}

void ImrAlgorithms::topology_generate(vector<RingTopologyTuple *> &topology) {

}
