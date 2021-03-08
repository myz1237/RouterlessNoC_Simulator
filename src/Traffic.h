#ifndef NOCSIM_TRAFFIC_H
#define NOCSIM_TRAFFIC_H
#include "Flit.h"
#include "GlobalParameter.h"
#include <cstdlib>
#include <cmath>
struct Packetinfo;
class Traffic{
public:

    int get_randomint(const int min, const int max)const;
    virtual Packetinfo* traffic_generator(const int local_id, const int curr_time) = 0;
};
class TrafficUniform: public Traffic{

    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
};

//以y=mesh_y - (mesh_y/mesh_x)x为对称轴
class TrafficTranspose1: public Traffic{
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
};

//以y = x为对称轴
class TrafficTranspose2:public Traffic{
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
};

class TrafficBitReverse:public Traffic{
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;

private:
    inline int getBit(int x, int w){return (x >> w) & 1;}
    inline double log2ceil(double x)const{return ceil(log(x) / log(2.0));}
    void setBit(int &x, int w, int v);
};

class TrafficHotspot:public Traffic{
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
};
//TODO SPLASH&PARSEC


#endif //NOCSIM_TRAFFIC_H
