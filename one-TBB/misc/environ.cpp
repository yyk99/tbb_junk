//
//
//


#include <tbb/tbb.h>
#include <iostream>

int main()
{
#ifdef __TBB_CPP14_INTEGER_SEQUENCE_PRESENT
  std::cout << "__TBB_CPP14_INTEGER_SEQUENCE_PRESENT" << " is defined\n";
  std::cout << "__TBB_CPP14_INTEGER_SEQUENCE_PRESENT" << " == " << __TBB_CPP14_INTEGER_SEQUENCE_PRESENT << "\n";
#else
  std::cout << "__TBB_CPP14_INTEGER_SEQUENCE_PRESENT" << " is NOT defined\n";
#endif
}