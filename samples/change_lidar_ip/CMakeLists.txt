cmake_minimum_required(VERSION 3.0)

set(DEMO_NAME change_lidar_ip)
add_executable(${DEMO_NAME} main.cpp)

target_link_libraries(${DEMO_NAME}
        PUBLIC
        livox_lidar_sdk_static
                                )
