/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Flit.h                                                          *
*  @brief    Four types of synthetic traffic patterns, uniform, transpose    *
*            bit reverse and hot spots.                                      *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

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
    int random_2size() const;
    Traffic(int node_sum);
    ~Traffic(){}
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
    ~TrafficTranspose(){}
    TrafficTranspose(int nodeSum);
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
};

class TrafficBitReverse:public Traffic{
public:
    ~TrafficBitReverse(){}
    TrafficBitReverse(int nodeSum);
    Packetinfo* traffic_generator(int local_id, const int curr_time) override;

private:
    inline int getBit(int x, int w){return (x >> w) & 1;}
    inline double log2ceil(double x)const{return ceil(log(x) / log(2.0));}
    void setBit(int &x, int w, int v);
};

class TrafficHotspot:public Traffic{
public:
    ~TrafficHotspot(){}
    TrafficHotspot(int nodeSum);
    Packetinfo* traffic_generator(const int local_id, const int curr_time) override;
private:
    int max_value;
    int get_dst();
};
//TODO SPLASH&PARSEC


#endif //NOCSIM_TRAFFIC_H
