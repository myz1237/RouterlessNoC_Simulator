#ifndef NOCSIM_TRAFFIC_H
#define NOCSIM_TRAFFIC_H
#include "Flit.h"
#include "GlobalParameter.h"
#include "RandomUtil.h"
#include <cstdlib>
#include <cmath>
struct Packetinfo;
class Traffic{
public:
    int m_node_sum;
    int get_randomsize() const;
    Traffic(int node_sum);
    virtual Packetinfo* traffic_generator(const int local_id, const int curr_time) = 0;
};
class TrafficUniform: public Traffic{
public:
    TrafficUniform(int nodeSum);
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
};

//以y=mesh_y - (mesh_y/mesh_x)x为对称轴
class TrafficTranspose: public Traffic{
private:
    int m_shift;
    inline void generate_shift();
public:
    TrafficTranspose(int nodeSum);
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
};

class TrafficBitReverse:public Traffic{
public:
    TrafficBitReverse(int nodeSum);
    Packetinfo* traffic_generator(int local_id, const int curr_time) override;

private:
    inline int getBit(int x, int w){return (x >> w) & 1;}
    inline double log2ceil(double x)const{return ceil(log(x) / log(2.0));}
    void setBit(int &x, int w, int v);
};

class TrafficHotspot:public Traffic{
public:
    TrafficHotspot(int nodeSum);
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
private:
    int max_value;
    int get_dst();
};
//TODO SPLASH&PARSEC


#endif //NOCSIM_TRAFFIC_H
