cmake_minimum_required(VERSION 3.10.2)

project("ffmpeg-jupiter")

find_library(log-lib log)

include_directories(src/main/cpp/include/)
file(GLOB CPP_DIR src/main/cpp/*.cpp)
add_library(ffmpeg-jupiter SHARED ${CPP_DIR})

set(LINK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI})
link_directories(${LINK_DIR})
file(GLOB SO_DIR ${LINK_DIR}/*.so)

target_link_libraries(
        ffmpeg-jupiter
        ${SO_DIR}
        jnigraphics
        ${log-lib})

#include_directories(src/main/cpp/include/libyuv/include/)
#add_subdirectory(src/main/cpp/include/libyuv/ ./build)
#aux_source_directory(src/main/cpp/include/libyuv/ YUV_CPP_DIR)
#add_library(yuvutil SHARED ${YUV_CPP_DIR})
#target_link_libraries(yuvutil ${log-lib} yuv)