cmake_minimum_required(VERSION 3.20)
project(FFlags C)
set(CMAKE_C_STANDARD 11)

option(BUILD_SHARED_LIBS "shared/static libs" ON)
option(FFLAGS_BUILD_TEST "build fflags test" ON)
option(SET_OUTPUT_DIRECTORY "set output directory" OFF)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(SET_OUTPUT_DIRECTORY)
    message(STATUS "set output directory")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)  # 设置输出路径
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
endif()

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    message("Build with MSVC")
else()
    add_compile_options(-fPIC)  # 启用fPIC参数
    message("Not build with MSVC")
endif()

include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")  # 设置安装路径

add_library(FFlags fflags.c)
set_target_properties(FFlags PROPERTIES PUBLIC_HEADER fflags.h)

if(FFLAGS_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

install(TARGETS FFlags
        LIBRARY DESTINATION lib  # 动态库安装路径
        ARCHIVE DESTINATION lib  # 静态库安装路径
        RUNTIME DESTINATION bin  # 可执行文件安装路径
        PUBLIC_HEADER DESTINATION include  # 头文件安装路径
        )