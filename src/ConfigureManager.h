/*****************************************************************************
*  Routerless Network-on-Chip Simulator                                      *
*                                                                            *
*  @file     ConfigureManager.h                                              *
*  @brief    This header file is to read key parameters from Yaml File       *
*                                                                            *
*                                                                            *
*  @author   Yizhuo Meng                                                     *
*  @email    myz2ylp@connect.hku.hk                                          *
*  @date     2020.03.16                                                      *
*                                                                            *
*****************************************************************************/

#ifndef NOCSIM_CONFIGUREMANAGER_H
#define NOCSIM_CONFIGUREMANAGER_H
#include <string>
#include <yaml-cpp/yaml.h>
#include <cmath>
#include "GlobalParameter.h"
#include "Traffic.h"
using namespace std;

/**
 * @brief Read parameters from Yaml file.
 * @param node           Yaml-Cpp Object for Yaml file access
 * @param param          String Field you want to read
 * @param default_value  Default values
 * @return               Values of field, default values if fail
 */
template<typename T>
T readParam(YAML::Node node, const string& param, T default_value);

/**
 * @brief Check Validation of node in HotSpot Traffic Pattern
 * @param test  Values read from Yaml File
 * @return      Exit the program if inappropriate values
 */
template<typename T>
void checkRange(T min, T max, T test);

/**
 * @brief Calculate average size of packets and set it as the size of EXB
 * @return      The size of EXB
 */
int get_exb_size(const int min, const int max);

void configure();


#endif //NOCSIM_CONFIGUREMANAGER_H
