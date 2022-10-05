# tbb_junk
Source code and examples for TBB

1. Linux intel compiler (PRO-TBB)

$ . ~/intel/sw_dev_tools/bin/iccvars.sh intel64
$ . ~/intel/sw_dev_tools/pstl/bin/pstlvars.sh intel64
$ cmake .. -DCMAKE_CXX_COMPILER=icc -DBUILD_PRO=ON

2. 

$ cmake .. -DCMAKE_CXX_COMPILER=clang++ -DBUILD_PRO=ON

Required:
MinGW:
	pacman -S mingw-w64-clang-x86_64-tbb
