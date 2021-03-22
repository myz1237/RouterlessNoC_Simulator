cmake_minimum_required(VERSION 3.5.1)
project(NocSim)

set(CMAKE_CXX_STANDARD 14)

include_directories(lib/yaml-cpp/include lib/plog/include)
link_directories(lib/yaml-cpp/lib)

add_executable( NocSim
                src/GlobalParameter.cpp src/GlobalParameter.h
                src/ConfigureManager.cpp src/ConfigureManager.h
                src/Node.cpp src/Node.h
                src/Ring.cpp src/Ring.h
                src/Noc.cpp src/Noc.h
                src/Node.cpp src/Node.h
                src/ExbManager.cpp src/ExbManager.h
                src/Flit.cpp src/Flit.h
                src/Injection.cpp src/Injection.h
                src/Util.h
                src/Traffic.cpp src/Traffic.h
                src/ringalgorithm/RingAlgorithms.cpp src/ringalgorithm/RingAlgorithms.h
                src/RoutingTable.cpp src/RoutingTable.h
                src/Main.cpp src/RandomUtil.h src/KnuthRngWrapper.cpp src/KunthRngDoubleWrapper.cpp src/Test.h)

target_link_libraries(NocSim libyaml-cpp.a)