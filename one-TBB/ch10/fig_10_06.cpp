/*
Copyright (C) 2019 Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*/

#include <iostream>
#include <tbb/task_group.h>
#include <tbb/tick_count.h>

int cutoff = 30;

long fib(long n) {
  if(n<2)
    return n;
  else
    return fib(n-1)+fib(n-2);
}

long parallel_fib(long n) {
  if(n<cutoff) {
    return fib(n);
  }
  else {
    long x, y;
    tbb::task_group g;
    g.run([&]{x=parallel_fib(n-1);}); // spawn a task
    g.run([&]{y=parallel_fib(n-2);}); // spawn another task
    g.wait();                // wait for both tasks to complete
    return x+y;
  }
}

int main(int argc, char** argv)
{
  int n = 30;
  int nth = 4;
//  tbb::task_scheduler_init init{nth};

  auto t0 = tbb::tick_count::now();
  long fib_s = fib(n);
  auto t1 = tbb::tick_count::now();
  long fib_p = parallel_fib(n);
  auto t2 = tbb::tick_count::now();
  double t_s = (t1 - t0).seconds();
  double t_p = (t2 - t1).seconds();

  std::cout << "SerialFib:   " << fib_s << " Time: " << t_s << "\n";
  std::cout << "ParallelFib: " << fib_p << " Time: " << t_p << " Speedup: " << t_s/t_p << "\n";
  return 0;
}

// g++ -o f fig_10_06.cpp -std=c++11 -O3 -ltbb -Wall -pedantic -ltbbmalloc_proxy
//./f 40
//SerialFib:   102334155 Time: 0.517525
//ParallelFib: 102334155 Time: 0.135017 Speedup: 3.83304
