
#include "Traffic.h"

Packetinfo* TrafficUniform::traffic_generator(const int local_id, const int curr_time) {
    Packetinfo *p = new Packetinfo;
    p->src = local_id;
    p->length = get_randomint(GlobalParameter::short_packet_size,
                              GlobalParameter::long_packet_size);
    p->ctime = curr_time;

    int max_id = GlobalParameter::mesh_dim_y*GlobalParameter::mesh_dim_x - 1 ;
    do{
        p->dst = get_randomint(0, max_id);
    }while(p->dst == p->src);

    return p;
}

Packetinfo* TrafficTranspose1::traffic_generator(const int local_id, const int curr_time) {
    Packetinfo *p = new Packetinfo;
    p->src = local_id;
    p->length = get_randomint(GlobalParameter::short_packet_size,
                              GlobalParameter::long_packet_size);
    p->ctime = curr_time;

    Coordinate src, dst;
    src.x = Coordinate::id_to_coor(p->src).x;
    src.y = Coordinate::id_to_coor(p->src).y;
    dst.x = GlobalParameter::mesh_dim_x - 1 - src.y;
    dst.y = GlobalParameter::mesh_dim_y - 1 - src.x;
    Coordinate::fix_range(dst);
    p->dst = Coordinate::coor_to_id(dst);
    return p;
}

Packetinfo* TrafficTranspose2::traffic_generator(const int local_id, const int curr_time) {
    Packetinfo *p = new Packetinfo;
    p->src = local_id;
    p->length = get_randomint(GlobalParameter::short_packet_size,
                              GlobalParameter::long_packet_size);
    p->ctime = curr_time;

    Coordinate src, dst;
    src.x = Coordinate::id_to_coor(p->src).x;
    src.y = Coordinate::id_to_coor(p->src).y;
    dst.x = src.y;
    dst.y = src.x;
    Coordinate::fix_range(dst);
    p->dst = Coordinate::coor_to_id(dst);
    return p;
}

Packetinfo* TrafficBitReverse::traffic_generator(const int local_id, const int curr_time) {
    Packetinfo *p = new Packetinfo;
    p->src = local_id;
    p->length = get_randomint(GlobalParameter::short_packet_size,
                              GlobalParameter::long_packet_size);
    p->ctime = curr_time;

    int nbits = (int)log2ceil((double)
                                      (GlobalParameter::mesh_dim_x * GlobalParameter::mesh_dim_y));
    int dnode = 0;
    for (int i = 0; i < nbits; i++){
        setBit(dnode, i, getBit(local_id, nbits - i - 1));
    }
    p->dst = dnode;
    return p;
}



Packetinfo* TrafficHotspot::traffic_generator(const int local_id, const int curr_time) {
    Packetinfo *p = new Packetinfo;
    double rnd = rand() / (double) RAND_MAX;
    double range_start = 0.0;
    p->src = local_id;
    p->length = get_randomint(GlobalParameter::short_packet_size,
                              GlobalParameter::long_packet_size);
    p->ctime = curr_time;

    int max_id = GlobalParameter::mesh_dim_y*GlobalParameter::mesh_dim_x - 1 ;
    do{
        p->dst = get_randomint(0, max_id);
        for (size_t i = 0; i < GlobalParameter::hotspot.size(); i++) {
            if (rnd >= range_start && rnd < range_start + GlobalParameter::hotspot[i].second) {
                if (local_id != GlobalParameter::hotspot[i].first ) {
                    p->dst = GlobalParameter::hotspot[i].first;
                }
                break;
            } else
                range_start += GlobalParameter::hotspot[i].second;	// try next
        }
    }while(p->dst == p->src);

    return p;
}
void TrafficBitReverse::setBit(int &x, int w, int v) {
    int mask = 1 << w;
    if (v == 1)
        x = x | mask;
    else if (v == 0)
        x = x & ~mask;
}
/*int Traffic::get_randomsize() const{
    // 1/(s_to_l+1)的概率产生长packet 其余产生短包
    int sum = GlobalParameter::short_percent + GlobalParameter::long_percent;
    double p = (double)GlobalParameter::short_percent/sum;
    double rnd = rand() / (double) RAND_MAX;
    //出在short的范围内[0,p]
    if(rnd >= 0 && rnd < p){
        return GlobalParameter::short_packet_size;
    } else {
    //出在[p,1) 随机产生2到GlobalParameter::long_packet_size
        return get_randomint(2, GlobalParameter::long_packet_size);
    }
}*/

int Traffic::get_randomint(const int min, const int max) const {
    return min +
           (int) ((double) (max - min + 1) * rand() / (RAND_MAX + 1.0));
}