# tbb_junk
Source code and examples for TBB

cmake .. -DCMAKE_CXX_COMPILER=icc

cmake .. -DCMAKE_CXX_COMPILER=clang++ -DBUILD_PRO=ON

Required:
MinGW:
	pacman -S mingw-w64-clang-x86_64-tbb
