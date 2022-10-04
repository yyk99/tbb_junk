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

#include <tbb/tbb.h>
#include <iostream>
#include <memory>

const int A_VERY_LARGE_NUMBER = 1<<12;

void spinWaitForAtLeast(double sec) {
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < sec);
}

void warmupTBB() {
  tbb::parallel_for(0, tbb::this_task_arena::max_concurrency(), 
  [](int) {
    spinWaitForAtLeast(0.001);
  });
}

std::atomic<int> bigObjectCount;
int maxCount = 0;

class BigObject {
   const int id;
   /* And a big amount of other data */
public:
   BigObject() : id(-1) { } 
   BigObject(int i) : id(i) { 
     int cnt = bigObjectCount++ + 1;
     if (cnt > maxCount) 
       maxCount = cnt;
   }
   BigObject(const BigObject& b) : id(b.id) { }
   virtual ~BigObject() {
     bigObjectCount--;
   }
   int get_id() const {return id;}
};

using BigObjectPtr = std::shared_ptr<BigObject>;

void fig_17_15() {
  tbb::flow::graph g;

  int src_count = 0;
  tbb::flow::input_node<BigObjectPtr>
  source{g, [&src_count] (tbb::flow_control &fc) -> BigObjectPtr {
      if (src_count < A_VERY_LARGE_NUMBER) {
        BigObjectPtr m = std::make_shared<BigObject>(src_count++);
        return m;
      }
      fc.stop();
      return {};
    }
  };
  tbb::flow::limiter_node<BigObjectPtr> limiter{g, 3};
  tbb::flow::function_node<BigObjectPtr, tbb::flow::continue_msg> 
  unlimited_node{g, tbb::flow::unlimited,
    [] (BigObjectPtr m) {
      spinWaitForAtLeast(0.0001);
      return tbb::flow::continue_msg();
    }
  };
  tbb::flow::make_edge(source, limiter);
  tbb::flow::make_edge(limiter, unlimited_node);
  tbb::flow::make_edge(unlimited_node, limiter.decrement);

  bigObjectCount = 0;
  maxCount = 0;
  source.activate();
  g.wait_for_all();
  std::cout << "maxCount == " << maxCount << std::endl;
}

int main(int argc, char *argv[]) {
  warmupTBB();
  tbb::tick_count t0 = tbb::tick_count::now();
  fig_17_15();
  tbb::tick_count t1 = tbb::tick_count::now();
  std::cout << "Total time == " << (t1-t0).seconds() << std::endl;
  return 0;
}

