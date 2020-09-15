#
#
#

project(tbb_junk LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set (PSTLROOT "/home/yyk/intel/sw_dev_tools/pstl" CACHE PATH "...")
set (TBBROOT "/home/yyk/intel/sw_dev_tools/tbb" CACHE PATH "...")
set (TBB_LIBRARY_DIR "/home/yyk/intel/sw_dev_tools/tbb/lib" CACHE PATH "...")

set (PSTL_INCLUDE ${PSTLROOT}/include)
set (TBB_INCLUDE ${TBBROOT}/include)

set (TBB_DEFINES TBB_SUPPRESS_DEPRECATED_MESSAGES)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

macro (add_fig directory)
  file(GLOB source_files "${directory}/*.cpp")
  #  list(REMOVE_ITEM ${source_files} pro-TBB/ch02/lodepng.h  pro-TBB/ch03/lodepng.h )
  foreach(cpp ${source_files})
    get_filename_component(dir ${cpp} DIRECTORY) 
    get_filename_component(dir ${dir} NAME) 
    get_filename_component(prog ${cpp} NAME_WE) 
    get_filename_component(src ${cpp} NAME)
    if(src STREQUAL lodepng.cpp)
      continue()
    endif()
    set(exe_name ${dir}_${prog})
    # message(STATUS "add_executable(${exe_name} ${src})")

    add_executable(${exe_name} ${cpp})
    target_compile_definitions(${exe_name} PRIVATE ${TBB_DEFINES})
    target_include_directories(${exe_name} PRIVATE ${PSTL_INCLUDE} ${TBB_INCLUDE})
    # target_link_directories(${exe_name} PRIVATE ${TBB_LIBRARY_DIR})
    target_link_libraries(${exe_name} ${TBB_libs})
  endforeach()
endmacro(add_fig)

link_directories(${TBB_LIBRARY_DIR})
set (TBB_libs "-ltbb -ltbbmalloc" Threads::Threads)

add_fig("pro-TBB/ch01")
add_fig("pro-TBB/ch02")
add_fig("pro-TBB/ch03")
add_fig("pro-TBB/ch04")
add_fig("pro-TBB/ch05")
add_fig("pro-TBB/ch06")
add_fig("pro-TBB/ch07")
add_fig("pro-TBB/ch08")
add_fig("pro-TBB/ch09")
add_fig("pro-TBB/ch10")
add_fig("pro-TBB/ch11")
add_fig("pro-TBB/AppB")

