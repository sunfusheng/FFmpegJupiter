cmake_minimum_required(VERSION 3.10.2)

project("ffmpeg-jupiter")

find_library(log-lib log)
find_library(jnigraphics-lib jnigraphics)

include_directories(src/main/cpp/include/)
file(GLOB CPP_DIR src/main/cpp/*.cpp)
add_library(ffmpeg-jupiter SHARED ${CPP_DIR})

set(LINK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI})
link_directories(${LINK_DIR})
file(GLOB SO_DIR ${LINK_DIR}/*.so)

target_link_libraries(
        ffmpeg-jupiter
        ${SO_DIR}
        log
        jnigraphics
)