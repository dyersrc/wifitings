file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(wifitings SHARED
    ${tool_dir}/cmd.cpp
    ${tool_dir}/int_tools.cpp 
    ${tool_dir}/mac_tools.cpp
    ${tool_dir}/time_tools.cpp
    ${tool_dir}/stringprt.cpp
    ${tool_dir}/llargekeymap.cpp
    ${sources})

add_dependencies(wifitings leelog)

target_link_libraries(wifitings 
    client_common
    leelog
    ${NL_LIB}
    ${PCAP_LIB}
    ${PTHREAD_LIB}
    ${BOOST_SYSTEM_LIB}
    ${BOOST_THREAD_LIB}
    )

install(TARGETS wifitings
    LIBRARY DESTINATION "/wifidefense/sensor")
