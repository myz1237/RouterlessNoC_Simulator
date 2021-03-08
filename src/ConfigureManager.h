#ifndef NOCSIM_CONFIGUREMANAGER_H
#define NOCSIM_CONFIGUREMANAGER_H
#include <string>
#include <yaml-cpp/yaml.h>
#include <cmath>
#include "GlobalParameter.h"
#include "Traffic.h"
using namespace std;


template<typename T>
T readParam(YAML::Node node, const string& param, T default_value);
template<typename T>
T readParam(YAML::Node node, const string& param1, const string& param2, T default_value);
template<typename T>
T readParam(YAML::Node node, const string& param);
template<typename T>
void checkRange(T min, T max, T test);

int get_exb_size(const int min, const int max);

void configure();


#endif //NOCSIM_CONFIGUREMANAGER_H
