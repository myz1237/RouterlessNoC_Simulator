/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     Traffic.h                                                          *
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

/**
 * @brief Base class of Traffic
 *        Each subclass needs to overwrite the function, traffic_generator()
 */
class Traffic{
public:
    /*The number of nodes*/
    int m_node_num;

    /**
     * @brief Randomly choose two types of size for a packet
     *        Two types of size:
     *                   GlobalParameter::short_packet_size
     *                   GlobalParameter::long_packet_size
     *        Possibility of each type:
     *                   short_ratio/(long_ratio + short_ratio)
     *                   long_ratio/(long_ratio + short_ratio)
     *        If GlobalParameter::method_size_generator is set 1, this function will be called
     */
    int random_2size()const;
    Traffic(int node_sum);
    virtual ~Traffic(){}
    virtual Packetinfo* traffic_generator(const int local_id) = 0;
};
class TrafficUniform: public Traffic{
public:
    TrafficUniform(int nodeSum);
    Packetinfo* traffic_generator(const int local_id) override;
};

class TrafficTranspose: public Traffic{
private:
    int m_shift;
    inline void generate_shift();
public:
    ~TrafficTranspose(){}
    TrafficTranspose(int nodeSum);
    /**
     * @brief Generate Transpose Traffic
     *        Codes which calculate destinations are copied from BookSim Program
     */
    Packetinfo* traffic_generator(const int local_id) override;
};

class TrafficBitReverse:public Traffic{
public:
    ~TrafficBitReverse(){}
    TrafficBitReverse(int nodeSum);
    /**
     * @brief Generate BitReverse Traffic
     *        Codes which calculate destinations are copied from BookSim Program
     */
    Packetinfo* traffic_generator(int local_id) override;

};

class TrafficHotspot:public Traffic{
public:
    ~TrafficHotspot(){}
    TrafficHotspot(int nodeSum);
    /**
     * @brief Generate Hotspot Traffic
     *        Codes which calculate destinations are copied from BookSim Program
     */
    Packetinfo* traffic_generator(const int local_id) override;
private:
    int max_value;
    int get_dst();
};
//TODO SPLASH&PARSEC


#endif //NOCSIM_TRAFFIC_H
