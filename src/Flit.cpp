#include "Flit.h"
#include "GlobalParameter.h"


int Flit::calc_flit_latency() const {
    return m_atime-m_ctime;
}

Flit::Flit(const long packet_id, const int src, const int dst, const FlitType type, const int seq, const int ctime,
           int hop, int curr_node, int atime): m_packet_id(packet_id),m_src_id(src), m_dst_id(dst),m_type(type),m_sequence(seq), m_ctime(ctime),
                                               m_atime(atime),m_hop(hop),m_curr_node(curr_node), m_status(Injecting){}

ControlFlit::ControlFlit(const long packet_id, const int src, const int dst, const FlitType type, const int seq,
                         const int ctime, int hop, int curr_node):Flit(packet_id, src, dst, type, seq, ctime, hop, curr_node) {
    m_routing.reserve(GlobalParameter::mesh_dim_x*GlobalParameter::mesh_dim_x);
}

ControlFlit::~ControlFlit() {
    free_vetor<Routingsnifer*>(m_routing);
}

void ControlFlit::update_routing_snifer() {
    Routingsnifer* r = new Routingsnifer;
    r->hop_count = m_hop;
    r->node_id = m_curr_node;
    m_routing.push_back(r);
}

Packet::Packet(long packet_id, int length, int src, int dst, int node, int ctime):
        m_packet_id(packet_id), m_length(length),m_src_id(src),m_dst_id(dst),
        m_curr_node(node),m_ctime(ctime){
    //Default arrivetime is -1, means not arrive
    m_flit.reserve(length);
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

Packet::Packet(long packet_id, int length, int src, int ctime):
            m_packet_id(packet_id),m_src_id(src),m_dst_id(src),m_length(length),m_ctime(ctime),
            m_curr_node(src) {
    m_flit.reserve(length);
    this->attach(new ControlFlit(packet_id, src,src,Control,0,ctime,0,src));
}

Packet::Packet(int src, Packetinfo *packetinfo):m_length(packetinfo->length),
        m_packet_id(packetinfo->id),m_src_id(packetinfo->src),m_dst_id(packetinfo->dst),m_curr_node(src),
        m_ctime(packetinfo->ctime){
    m_flit.reserve(m_length);
    this->attach(new Flit(m_packet_id, m_src_id,m_dst_id,Header,0,m_ctime,0,m_curr_node));
    if(m_length == 2){
        //2-flit packet: Header+Tail
        this->attach(new Flit(m_packet_id, m_src_id,m_dst_id,Tail,1,m_ctime,0,m_curr_node));
    } else if(m_length > 1){
        //For 3 or more flits packet
        //Add payload flit into the packet
        for(int i=1; i<=m_length-2; i++){
            this->attach(new Flit(m_packet_id, m_src_id,m_dst_id,Payload,i,m_ctime,0,m_curr_node));
        }
        this->attach(new Flit(m_packet_id, m_src_id,m_dst_id,Tail,m_length-1, m_ctime,0,m_curr_node));
    }
}


Packet::~Packet() {
    //注意m_flit有时候是空的 free的时候一定要判断vector是否为空
    free_vetor<Flit*>(m_flit);
}

//Add one flit into the packet
void Packet::attach(Flit *flit) {
    m_flit.push_back(flit);
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
    out << "Packet Information: " << "ID: " << p.id
    << " Source:" << p.src << "  " << "Destination:" << p.dst <<
    "  "<< "Length:" << p.length << "  "<< "Creation Time:" << p.ctime;
    return out;
}


