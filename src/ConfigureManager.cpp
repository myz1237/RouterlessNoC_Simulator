#include "ConfigureManager.h"


YAML::Node conf;

void configure() {

    cout << "Load Configuration file.." << endl;
    try {
        conf = YAML::LoadFile("../configure/configure.yaml");
        cout << "Done" << endl;
    }catch(YAML::BadFile &e){
        cout << " Failed" << endl;
        cerr << "Error: The specified YAML configuration file was not found!" << endl;
        exit(0);
    } catch (YAML::ParserException &pe){
        cout << " Failed" << endl;
        cerr << "ERROR at line " << pe.mark.line +1 << " column " << pe.mark.column + 1 << ": "<< pe.msg << ". Please check indentation." << endl;
        exit(0);
    }

    GlobalParameter::mesh_dim_x = readParam<int>(conf,"mesh_dim_x",2);
    GlobalParameter::mesh_dim_y = readParam<int>(conf,"mesh_dim_y",2);
    GlobalParameter::injection_rate = readParam<double>(conf,"injection_rate",0.02);;
    GlobalParameter::long_packet_size = readParam<int>(conf,"long_packet_size",5);
    GlobalParameter::short_packet_size = readParam<int>(conf,"short_packet_size",1);
    GlobalParameter::short_packet_ratio = readParam<int>(conf,"short_ratio",8);
    GlobalParameter::long_packet_ratio = readParam<int>(conf,"long_ratio",2);
    GlobalParameter::flit_size = readParam<int>(conf,"flit_size",256);
    //TODO handle relationship between bandwidth and flit_size
    GlobalParameter::bandwidth = readParam<int>(conf,"bandwidth",256);
    string ring_strategy = readParam<string>(conf,"ring_strategy","RLrec");

    //GlobalParameter::ring_constraint = readParam<int>(conf,"ring_constraint",6);

    string exb_strategy = readParam<string>(conf,"exb_strategy","Max");
    GlobalParameter::exb_num = readParam<int>(conf,"exb_num",2);
    GlobalParameter::in_port_size = readParam<int>(conf,"in_port_size",1);
    GlobalParameter::ej_port_nu = readParam<int>(conf,"ej_port_nu",1);
    string ej_strategy = readParam<string>(conf,"ej_strategy","Oldest");
    string routing_strategy = readParam<string>(conf, "routing_strategy", "Shortest");
    string traffic_type = readParam<string>(conf,"traffic_type","Uniform");
    GlobalParameter::sim_time = readParam<int>(conf,"sim_time",10000);
    GlobalParameter::sim_warmup = readParam<int>(conf,"sim_warmup",1000);
    string sim_type = readParam<string>(conf,"sim_type","Latency");
    string latency_type = readParam<string>(conf,"latency_type","Both");
    //GlobalParameter::short_percent = readParam<int>(conf,"s_to_l","short",2);
    //GlobalParameter::long_percent = readParam<int>(conf,"s_to_l","long",1);

    GlobalParameter::sim_detail = readParam<int>(conf,"sim_detail",0);

    if(ring_strategy == "IMR") GlobalParameter::ring_strategy = IMR;
    else if(ring_strategy == "RLrec") GlobalParameter::ring_strategy = RLrec;

    if(exb_strategy == "Max") GlobalParameter::exb_strategy = Max;
    else if(exb_strategy == "Avg") GlobalParameter::exb_strategy = Avg;

    //TODO 还没有设定好
    if(ej_strategy == "Oldest") GlobalParameter::ej_strategy == Oldest;
    else if(ej_strategy == "Prio") GlobalParameter::ej_strategy == Prio;

    if(routing_strategy == "Shortest") GlobalParameter::routing_strategy = Shortest;
    else if(routing_strategy == "Secondwinner") GlobalParameter::routing_strategy = Secondwinner;

    if(traffic_type == "Uniform") GlobalParameter::traffic_type = Uniform;
    else if(traffic_type == "Transpose") GlobalParameter::traffic_type = Transpose;
    else if(traffic_type == "BitReverse") GlobalParameter::traffic_type = BitReverse;
    else if(traffic_type == "Hotspot") GlobalParameter::traffic_type = Hotspot;

    if(sim_type == "Latency") GlobalParameter::sim_type = Latency;
    else if(sim_type == "Throughput") GlobalParameter::sim_type = Throughput;

    if(latency_type == "Packets") GlobalParameter::latency_type = Packets;
    else if(latency_type == "Flits") GlobalParameter::latency_type = Flits;
    else if(latency_type == "Both") GlobalParameter::latency_type = Both;

    //Decide exb parameters according to exb strategy
    switch (GlobalParameter::exb_strategy) {
        case Max:
            GlobalParameter::exb_size = GlobalParameter::long_packet_size;
            break;
        case Avg:
            GlobalParameter::exb_size = get_exb_size(GlobalParameter::short_packet_size, GlobalParameter::long_packet_size);
            break;

    }

    //Mesh_DIM Check
    if(GlobalParameter::mesh_dim_x <= 1 || GlobalParameter::mesh_dim_y <=1){
        cerr << "Error: Mesh_dim_x and mesh_dim_y must be greater than 1" << endl;
        exit(0);
    }
    //Check NoC Size if ring strategy RLrec is choosen
    if(GlobalParameter::ring_strategy == RLrec){
        if(GlobalParameter::mesh_dim_x%2 || GlobalParameter::mesh_dim_y%2){
            cerr << "Error: Mesh_dim_x and mesh_dim_y mush be even in Algorithm RLrec" <<endl;
            exit(0);
        }
    }

    //Check simulation time and Warmup time
    if(GlobalParameter::sim_time < GlobalParameter::sim_warmup){
        cerr << "Error: Sim_time must be greater than sim_warmup" << endl;
        exit(0);
    }

    //Check s_to_l
/*    if(GlobalParameter::short_percent < 0){
        cerr << "Error: short_percent must be greater than 0" << endl;
        exit(0);
    } else if (GlobalParameter::long_percent < 0){
        cerr << "Error: long_percent must be greater than 0" << endl;
        exit(0);
    }*/

    if(GlobalParameter::traffic_type == Hotspot){
        int node;
        double p;
        for(YAML::const_iterator it= conf["hotspot"].begin(); it != conf["hotspot"].end();++it)
        {
            node = it->first.as<int>();
            p = it->second.as<int>();
            checkRange<int>(0, GlobalParameter::mesh_dim_x*GlobalParameter::mesh_dim_y - 1, node);
            pair<int,int>t(node,p);
            GlobalParameter::hotspot.push_back(t);
        }
    }
    int node_sum = GlobalParameter::mesh_dim_y*GlobalParameter::mesh_dim_x;
    //根据设置初始化各个对象

    if(GlobalParameter::traffic_type == Uniform){
        GlobalParameter::traffic = new TrafficUniform(node_sum);
    }else if(GlobalParameter::traffic_type == Transpose){
        GlobalParameter::traffic = new TrafficTranspose(node_sum);
    }else if(GlobalParameter::traffic_type == BitReverse){
        GlobalParameter::traffic = new TrafficBitReverse(node_sum);
    }else if(GlobalParameter::traffic_type == Hotspot){
        GlobalParameter::traffic = new TrafficHotspot(node_sum);
    }

    GlobalParameter::sim_time += GlobalParameter::sim_warmup;
}

template <typename T>
T readParam(YAML::Node node, const string& param, T default_value) {
    try {
        return node[param].as<T>();
    } catch(exception &e) {
        /*
        cerr << "WARNING: parameter " << param << " not present in YAML configuration file." << endl;
        cerr << "Using command line value or default value " << default_value << endl;
         */
        return default_value;
    }
}

template<typename T>
T readParam(YAML::Node node, const string& param1, const string& param2, T default_value){
    try {
        return node[param1][param2].as<T>();
    } catch(exception &e) {
        /*
        cerr << "WARNING: parameter " << param << " not present in YAML configuration file." << endl;
        cerr << "Using command line value or default value " << default_value << endl;
         */
        return default_value;
    }
}

template <typename T>
T readParam(YAML::Node node, const string& param) {
    try {
        return node[param].as<T>();
    } catch(exception &e) {
        cerr << "ERROR: Cannot read param " << param << ". " << endl;
        exit(0);
    }
}
template<typename T>
void checkRange(T min, T max, T test){
    if(test < min || test > max){
        cerr << "ERROR: Hopspot values are out of range. " << endl;
        exit(0);
    }
}

int get_exb_size(const int min, const int max){
    int num = max-min+1;
    int sum = 0;
    for (int i=min;i<=max;i++){
        sum += i;
    }
    return (int)ceil((double)sum/num);
}