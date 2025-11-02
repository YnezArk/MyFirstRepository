cmake_minimum_required(VERSION 3.31)
project(Cproject3 C)

set(CMAKE_C_STANDARD 17)

file(GLOB_RECURSE SRCS "${CMAKE_SOURCE_DIR}/*.c")

# 给每个 .c 文件生成一个对应的可执行目标
foreach(src ${SRCS})
    # 去掉路径和后缀，当成目标名
    get_filename_component(name ${src} NAME_WE)
    add_executable(${name} ${src})
endforeach()
