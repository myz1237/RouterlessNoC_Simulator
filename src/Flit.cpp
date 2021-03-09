#include "Flit.h"


Packet::Packet(long packet_id, int length, int src, int dst, int node, int ctime, bool finish):
        m_packet_id(packet_id), m_length(length),m_src_id(src),m_dst_id(dst),
        m_curr_node(node),m_ctime(ctime), m_finish(finish){
    //Default arrivetime is -1, means not arrive
    this->attach(new Flit(packet_id, m_src_id,m_dst_id,Header,0,m_ctime,0,m_curr_node));
    if(m_length == 2){
        //2-flit packet: Header+Tail
        this->attach(new Flit(packet_id, m_src_id,m_dst_id,Tail,1,m_ctime,0,m_curr_node));
    } else if(m_length > 1){
        //For 3 or more flits packet
        //Add payload flit into the packet
        for(int i=1; i<=m_length-2; i++){
            this->attach(new Flit(packet_id, m_src_id,m_dst_id,Payload,i,m_ctime,0,m_curr_node));
        }
        this->attach(new Flit(packet_id, m_src_id,m_dst_id,Tail,m_length-1, m_ctime,0,m_curr_node));
    }
}

Packet::Packet(long packet_id, int length, int src, int ctime, bool finish):
            m_packet_id(packet_id),m_src_id(src),m_dst_id(src),m_length(length),m_ctime(ctime),
            m_curr_node(src),m_finish(finish) {
    this->attach(new Flit(packet_id, src,src,Control,0,ctime,0,src));
}

Packet::Packet(long packet_id, int src, Packetinfo *packetinfo, bool finish):m_length(packetinfo->length),
        m_packet_id(packet_id),m_src_id(packetinfo->src),m_dst_id(packetinfo->dst),m_curr_node(src),
        m_ctime(packetinfo->ctime),m_finish(finish){
    this->attach(new Flit(packet_id, m_src_id,m_dst_id,Header,0,m_ctime,0,m_curr_node));
    if(m_length == 2){
        //2-flit packet: Header+Tail
        this->attach(new Flit(packet_id, m_src_id,m_dst_id,Tail,1,m_ctime,0,m_curr_node));
    } else if(m_length > 1){
        //For 3 or more flits packet
        //Add payload flit into the packet
        for(int i=1; i<=m_length-2; i++){
            this->attach(new Flit(packet_id, m_src_id,m_dst_id,Payload,i,m_ctime,0,m_curr_node));
        }
        this->attach(new Flit(packet_id, m_src_id,m_dst_id,Tail,m_length-1, m_ctime,0,m_curr_node));
    }
}


Packet::~Packet() {
    //注意m_flit有时候是空的 free的时候一定要判断vector是否为空
    free_vetor<Flit*>(m_flit);
}

Flit::~Flit() {
    free_vetor<Routingsnifer*>(m_routing);
}

void Flit::update_routing_snifer() {
    Routingsnifer* r = new Routingsnifer;
    r->hop_count = m_hop;
    r->node_id = m_curr_node;
    m_routing.push_back(r);
}

int Flit::calc_flit_latency() const {
    return m_atime-m_ctime;
}

//Add one flit into the packet
void Packet::attach(Flit *flit) {
    m_flit.push_back(flit);
}

inline
int Packet::calc_packet_latency() const{
    return m_flit[m_length-1]->m_atime-m_ctime;
}


inline
double Packet::calc_avg_flit_latency() {

    double latency = 0;
    double avg;
    for(int i=0;i<m_length-1;i++){
       latency += m_flit[i]->calc_flit_latency();
    }
    avg = latency / m_length;
    return avg;
}



int Coordinate::coor_to_id(const Coordinate &coor) {
    return (GlobalParameter::mesh_dim_y-coor.y-1)*GlobalParameter::mesh_dim_x+coor.x;
}

Coordinate Coordinate::id_to_coor(const int id) {
    return Coordinate(id%GlobalParameter::mesh_dim_x,
                      (GlobalParameter::mesh_dim_y-id)/GlobalParameter::mesh_dim_y);
}

void Coordinate::fix_range(Coordinate &dst) {
    //Transpose1
    if (dst.x < 0)
        dst.x = 0;
    if (dst.y < 0)
        dst.y = 0;
    //Transpose2
    if (dst.x >= GlobalParameter::mesh_dim_x)
        dst.x = GlobalParameter::mesh_dim_x - 1;
    if (dst.y >= GlobalParameter::mesh_dim_y)
        dst.y = GlobalParameter::mesh_dim_y - 1;
}



ostream& operator<<(ostream& out, Packet& p){
    out << "Packet Content:" << endl
    << "Length:" << p.get_length() << "  " << "Source:" << p.get_src()
    << "  " << "Destination:" << p.get_dst() << "  " << "Creation Time:" << p.get_ctime() << endl;
    for(int i=0;i<p.get_length();i++){
        out << "Flit " << i << "  " << p.get_flit_type(i) << "Current Node:" <<  p.get_flit_curr_node(i)
        << "  Hop Count:" << p.get_flit_hop(i) << "  Arrival Time:" << p.get_flit_atime(i) << endl;
    }
    return out;
}

ostream& operator<<(ostream& out, Packetinfo& p){
    out << "Packet Information:" << endl
    << "Source:" << p.src << "  " << "Destination:" << p.dst <<
    "  "<< "Length:" << p.length << "  "<< "Creation Time:" << p.ctime << endl;
    return out;
}

