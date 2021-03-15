#include "GlobalParameter.h"

int GlobalParameter::mesh_dim_x;
int GlobalParameter::mesh_dim_y;
double GlobalParameter::injection_rate;
int GlobalParameter::long_packet_size;
int GlobalParameter::short_packet_size;
int GlobalParameter::long_packet_ratio;
int GlobalParameter::short_packet_ratio;
int GlobalParameter::flit_size;
int GlobalParameter::bandwidth;
RingStrategy GlobalParameter::ring_strategy;
//int GlobalParameter::ring_constraint;
ExbStrategy GlobalParameter::exb_strategy;
int GlobalParameter::in_port_size;
int GlobalParameter::ej_port_nu;
EjStrategy GlobalParameter::ej_strategy;
RoutingStrategy GlobalParameter::routing_strategy;
TrafficType GlobalParameter::traffic_type;
vector<pair<int, int>> GlobalParameter::hotspot;
int GlobalParameter::sim_time;
int GlobalParameter::sim_warmup;
SimType GlobalParameter::sim_type;
LatencyType GlobalParameter::latency_type;
int GlobalParameter::sim_detail;
int GlobalParameter::exb_num;

vector<Ring*> GlobalParameter::ring;
Traffic* GlobalParameter::traffic;
RingAlgorithms* GlobalParameter::ring_algorithm;
int GlobalParameter::exb_size;

//记录当前发送packet的id号 全局唯一性
volatile long GlobalParameter::packet_id = 0;

long GlobalParameter::global_cycle = 0;

int GlobalParameter::error = 0;

