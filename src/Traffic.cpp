
#include "Traffic.h"

Traffic::Traffic(int node_sum):m_node_sum(node_sum){}

Packetinfo* TrafficUniform::traffic_generator(const int local_id, const int curr_time) {
    Packetinfo *p = new Packetinfo;
    p->src = local_id;
    //p->length = get_randomsize();
    p->length = random_int(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);
    p->ctime = curr_time;
    //cout << p->length << endl;
    int max_id = GlobalParameter::mesh_dim_y*GlobalParameter::mesh_dim_x - 1 ;
    do{
        p->dst = random_int(0, max_id);
    }while(p->dst == p->src);
    GlobalParameter::packet_id++;
    return p;
}

TrafficUniform::TrafficUniform(int nodeSum) : Traffic(nodeSum) {}

Packetinfo* TrafficTranspose::traffic_generator(const int local_id, const int curr_time) {
    int const mask_lo = (1 << m_shift) - 1;
    int const mask_hi = mask_lo << m_shift;
    Packetinfo *p = new Packetinfo;
    p->src = local_id;
    p->length = random_int(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);
    p->ctime = curr_time;
    p->dst = (((local_id >> m_shift) & mask_lo) | ((local_id << m_shift) & mask_hi));
    GlobalParameter::packet_id++;
    return p;
}

void TrafficTranspose::generate_shift() {
    while(m_node_sum >>= 1) {
        ++m_shift;
    }
    if(m_shift % 2) {
        cout << "Error: Transpose traffic pattern requires the number of nodes to "
             << "be an even power of two." << endl;
        exit(-1);
    }
    m_shift >>= 1;
}

TrafficTranspose::TrafficTranspose(int nodeSum) : Traffic(nodeSum), m_shift(0) {
    generate_shift();
}


Packetinfo* TrafficBitReverse::traffic_generator(int local_id, const int curr_time) {
    int dst = 0;
    Packetinfo *p = new Packetinfo;
    p->src = local_id;
    p->length = random_int(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);

    p->ctime = curr_time;

    for(int n = m_node_sum; n > 1; n >>= 1) {
        dst = (dst << 1) | (local_id % 2);
        local_id >>= 1;
    }

    p->dst = dst;
    GlobalParameter::packet_id++;
    return p;
}



Packetinfo* TrafficHotspot::traffic_generator(const int local_id, const int curr_time) {
    Packetinfo *p = new Packetinfo;
    double rnd = rand() / (double) RAND_MAX;
    double range_start = 0.0;
    p->src = local_id;
    p->length = get_dst();
    p->ctime = curr_time;

    GlobalParameter::packet_id++;
    return p;
}

TrafficHotspot::TrafficHotspot(int nodeSum) : Traffic(nodeSum) {
    max_value = 0;
    int size = GlobalParameter::hotspot.size();
    for(int i = 0; i < size; i++){
        max_value += GlobalParameter::hotspot.at(i).second;
    }
}

int TrafficHotspot::get_dst() {
    int size = GlobalParameter::hotspot.size();
    if(size == 1) {
        return GlobalParameter::hotspot[0].first;
    }

    int pct = random_int(0,max_value);

    for(size_t i = 0; i < (size - 1); ++i) {
        int const limit = GlobalParameter::hotspot.at(i).second;
        if(limit > pct) {
            return GlobalParameter::hotspot.at(i).first;
        } else {
            pct -= limit;
        }
    }
    return GlobalParameter::hotspot.back().first;
}


TrafficBitReverse::TrafficBitReverse(int nodeSum) : Traffic(nodeSum) {}

int Traffic::get_randomsize() const{
    // 1/(s_to_l+1)的概率产生长packet 其余产生短包
    int sum = GlobalParameter::short_packet_ratio + GlobalParameter::long_packet_ratio;
    double p = (double)GlobalParameter::short_packet_ratio/sum;
    double rnd = rand() / (double) RAND_MAX;
    //出在long的范围内[0,p]
    if(rnd >= 0 && rnd < p){
        return GlobalParameter::long_packet_size;
    } else {
    //出在[p,1) 随机产生2到GlobalParameter::long_packet_size
        return GlobalParameter::short_packet_size;
    }
}

