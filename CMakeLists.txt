#
#
#

project(tbb_junk LANGUAGES CXX)
cmake_minimum_required(VERSION 3.10)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenCL )
find_package(TBB REQUIRED)

option(BUILD_PRO "Build with pro-TBB" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

CHECK_INCLUDE_FILE_CXX("hwloc.h" HAS_HWLOC)
if ( NOT HAS_HWLOC )
  message(WARNING "hwloc.h is not found. Some examples will not be compiled")
else()
  set(HWLOC_libs "-lhwloc")
endif()

list (APPEND skip_source
  lodepng.cpp
  fig_18_11-CLheader.cpp
  fig_18_11-svmCLheader.cpp
  )

if(NOT OpenCL_FOUND)
  message(WARNING "OpenCL is not found. Some examples will not be built")
  list(APPEND skip_source
    fig_18_11-svm.cpp
    fig_18_11.cpp
    )
endif()

list(APPEND tbbmalloc_required
  fig_B_48_fixed_pool.cpp
  fig_B_48_memory_pool.cpp
  fig_7_15and16and17.cpp
)

list(APPEND hwloc_required
  fig_20_06.cpp
  fig_20_10.cpp
)

macro (add_fig directory)
  file(GLOB source_files "${directory}/*.cpp")
  #  list(REMOVE_ITEM ${source_files} pro-TBB/ch02/lodepng.h  pro-TBB/ch03/lodepng.h )
  foreach(cpp ${source_files})
    get_filename_component(dir ${cpp} DIRECTORY)
    get_filename_component(dir ${dir} NAME)
    get_filename_component(prog ${cpp} NAME_WE)
    get_filename_component(src ${cpp} NAME)
    if(src IN_LIST skip_source)
      continue()
    endif()
    set(exe_name ${dir}_${prog})
    # message(STATUS "add_executable(${exe_name} ${src})")

    add_executable(${exe_name} ${cpp})
    target_link_libraries(${exe_name} PRIVATE TBB::tbb)
    if (src IN_LIST tbbmalloc_required)
      target_link_libraries(${exe_name} PRIVATE TBB::tbbmalloc)
    endif()
    if (src IN_LIST hwloc_required)
      target_link_libraries(${exe_name} PRIVATE hwloc)
    endif()
    target_compile_features(${exe_name} PRIVATE cxx_std_11)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_compile_options(${exe_name} PRIVATE "-Wno-deprecated-declarations")
    endif()
    target_compile_definitions(${exe_name} PRIVATE "-DTBB_SUPPRESS_DEPRECATED_MESSAGES=1")
    set_target_properties(${exe_name}
      PROPERTIES FOLDER "${dir}"
    )
  endforeach()
endmacro(add_fig)

set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

if (BUILD_PRO)
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
	add_fig("pro-TBB/ch12")
	add_fig("pro-TBB/ch13")
	add_fig("pro-TBB/ch14")
	add_fig("pro-TBB/ch15")
	add_fig("pro-TBB/ch16")
	add_fig("pro-TBB/ch17")
	add_fig("pro-TBB/ch18")
	if(OpenCL_FOUND)
	  add_fig("pro-TBB/ch19")
	endif()
	add_fig("pro-TBB/ch20")
	add_fig("pro-TBB/AppB")
else()
	message(STATUS "Building from one-TBB")
	add_fig("one-TBB/ch01")
	add_fig("one-TBB/ch02")
	add_fig("one-TBB/ch03")
	add_fig("one-TBB/ch04")
	add_fig("one-TBB/ch05")
	add_fig("one-TBB/ch06")
	add_fig("one-TBB/ch07")
	add_fig("one-TBB/ch08")
	add_fig("one-TBB/ch09")
	add_fig("one-TBB/ch10")
	add_fig("one-TBB/ch11")
	add_fig("one-TBB/ch12")
	add_fig("one-TBB/ch13")
	add_fig("one-TBB/ch14")
	add_fig("one-TBB/ch15")
	add_fig("one-TBB/ch16")
	add_fig("one-TBB/ch17")
	add_fig("one-TBB/ch18")
	if(OpenCL_FOUND)
	  add_fig("one-TBB/ch19")
	endif()
	add_fig("one-TBB/ch20")
	add_fig("one-TBB/AppB")
endif()

add_fig("one-TBB/misc")

set(SOLUTION_FILES
  .editorconfig
  )
add_custom_target( GlobalFiles SOURCES ${SOLUTION_FILES})

