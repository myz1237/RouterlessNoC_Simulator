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

    /*The number of nodes in NoC*/
    int m_node_num;

    /**
     * @brief Generate the size of the next packet according to your settings
     *        Only available when method_size_generator in Yaml File is set 1
     */
    int get_next_packet_size()const;

    /**
    * @brief Calculate the average packet size according to your settings
    */
    double get_avg_packet_size()const;

    /**
    * @brief Control whether to generate a packet infor
    *        Get a random Double Value
     *       Return true, if it is smaller than injection rate
    */
    bool time_to_generate_packetinfor()const;

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


#endif //NOCSIM_TRAFFIC_H
