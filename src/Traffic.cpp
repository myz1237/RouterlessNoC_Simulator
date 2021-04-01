
#include "Traffic.h"

Traffic::Traffic(int node_sum): m_node_num(node_sum){}

Packetinfo* TrafficUniform::traffic_generator(const int local_id) {
    if(time_to_generate_packetinfor()){
        Packetinfo *p = new Packetinfo;
        p->src = local_id;
        p->id = GlobalParameter::packet_id++;
        if(GlobalParameter::method_size_generator){
            p->length = get_next_packet_size();
        }else{
            p->length = random_int(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);
        }
        do{
            p->dst = random_int(0, m_node_num - 1);
        } while (p->dst == local_id);
        //p->dst = random_int(0, m_node_num - 1);

        return p;
    }else{
        return nullptr;
    }

}

TrafficUniform::TrafficUniform(int nodeSum) : Traffic(nodeSum) {}

Packetinfo* TrafficTranspose::traffic_generator(const int local_id) {
    if(time_to_generate_packetinfor()) {
        int const mask_lo = (1 << m_shift) - 1;
        int const mask_hi = mask_lo << m_shift;
        Packetinfo *p = new Packetinfo;
        p->src = local_id;

        if (GlobalParameter::method_size_generator) {
            p->length = get_next_packet_size();
        } else {
            p->length = random_int(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);
        }
        p->dst = (((local_id >> m_shift) & mask_lo) | ((local_id << m_shift) & mask_hi));
        if(p->dst == local_id) return nullptr;
        p->id = GlobalParameter::packet_id++;
        return p;
    }else{
        return nullptr;
    }
}

void TrafficTranspose::generate_shift() {
    while(m_node_num >>= 1) {
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


Packetinfo* TrafficBitReverse::traffic_generator(int local_id) {
    if(time_to_generate_packetinfor()) {
        int dst = 0;
        Packetinfo * p = new Packetinfo;
        p->src = local_id;

        if (GlobalParameter::method_size_generator) {
            p->length = get_next_packet_size();
        } else {
            p->length = random_int(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);
        }

        for (int n = m_node_num; n > 1; n >>= 1) {
            dst = (dst << 1) | (local_id % 2);
            local_id >>= 1;
        }
        p->dst = dst;
        if(p->dst == local_id) return nullptr;
        p->id = GlobalParameter::packet_id++;
        return p;
    }else{
        return nullptr;
    }
}



Packetinfo* TrafficHotspot::traffic_generator(const int local_id) {
    if(time_to_generate_packetinfor()) {
        Packetinfo *p = new Packetinfo;

        p->src = local_id;

        if (GlobalParameter::method_size_generator) {
            p->length = get_next_packet_size();
        } else {
            p->length = random_int(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);
        }
        p->dst = get_dst();
        if(p->dst == local_id) return nullptr;
        p->id = GlobalParameter::packet_id++;
        return p;
    }else{
        return nullptr;
    }
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
        int const limit = GlobalParameter::hotspot[i].second;
        if(limit > pct) {
            return GlobalParameter::hotspot[i].first;
        } else {
            pct -= limit;
        }
    }
    return GlobalParameter::hotspot.back().first;
}


TrafficBitReverse::TrafficBitReverse(int nodeSum) : Traffic(nodeSum) {}

int Traffic::get_next_packet_size() const{

    if(GlobalParameter::method_size_generator){
        vector<pair<int, int>> const & psize = GlobalParameter::packet_size_with_ratio;
        int size = psize.size();

        if(size == 1){
            return psize[0].first;
        }

        int pct = random_int(0,GlobalParameter::psum);

        for(int i = 0; i < (size - 1); ++i) {
            int const limit = psize[i].second;
            if(limit > pct) {
                return psize[i].first;
            } else {
                pct -= limit;
            }
        }
        return psize.back().first;
    } else{
        cerr << "Please set 1 to method_size_generator" << endl;
    }
}

double Traffic::get_avg_packet_size() const {
    if(GlobalParameter::method_size_generator){
        /*Calculate Avg packet size using weight value*/
        vector<pair<int, int>> const & psize = GlobalParameter::packet_size_with_ratio;
        int size = psize.size();

        if(size == 1){
            return (double)psize[0].first;
        }

        int sum = 0;
        for(int i = 0; i < size; i++){
            sum += psize[i].first * psize[i].second;
        }

        return (double)sum / (double)GlobalParameter::psum;

    }else{
        /*Simply calculate average size by adding*/
        int pshort = GlobalParameter::short_packet_size;
        int plong = GlobalParameter::long_packet_size;
        double sum = 0;
        for(int i = pshort; i <= plong; i++){
            sum += i;
        }
        return (double )sum / (double)(plong - pshort + 1);
    }

}

bool Traffic::time_to_generate_packetinfor() const {
    /*flit/node/cycle*/
    double injection_rate = GlobalParameter::injection_rate;
    double avg_packet_size = get_avg_packet_size();
    return (next_random_double() < (injection_rate / avg_packet_size));
}



