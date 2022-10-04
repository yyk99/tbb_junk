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
#include <map>
#include <set>
#include <vector>
#include <tbb/tbb.h>

const int P = tbb::this_task_arena::max_concurrency();
thread_local int my_tid = -1;
void doWork(const std::string& name, double seconds);

void fig_11_5() {
  int N = 10*P;
  tbb::parallel_for(0, N, [](int) { doWork("1st pfor", 0.01); });

  tbb::flow::graph g;
  /* construct and use graph */
  int cnt = 0;
  tbb::flow::input_node<int> src(g, 
    [&](tbb::flow_control& fc) -> bool {
      doWork("flow graph", 0);
      if (cnt < N) {
        auto i = cnt++;
        return i;
      } 
      fc.stop();
      return {};
    }
  );
  tbb::flow::function_node<int> f(g, tbb::flow::unlimited, 
    [&](const int& i) {
      doWork("flow graph", 0.01);
    }
  );
  tbb::flow::make_edge(src,f);
  src.activate();
  g.wait_for_all();

  tbb::parallel_for(0, N, [](int) { doWork("2nd pfor", 0.01); });
}

std::vector<std::set<std::string>> tid_regions(3*P);
std::atomic<int> next_tid;

void noteParticipation(const std::string& name) {
  if (my_tid == -1) {
    //my_tid = next_tid.fetch_and_increment();
    my_tid = next_tid++;
  }
  tid_regions[my_tid].insert(name);
}

void doWork(const std::string& name, double seconds) {
  noteParticipation(name);
  tbb::tick_count t0 = tbb::tick_count::now();
  while ((tbb::tick_count::now() - t0).seconds() < seconds);
}

void dump_participation() {
  int end = next_tid;
  std::map<std::string, int> m;
  for (int i = 0; i < end; ++i) {
    for (auto n : tid_regions[i]) {
      m[n] += 1;
    }
  }
  std::cout << "There are " << tbb::this_task_arena::max_concurrency() << " logical cores." << std::endl;
  for (auto& kv : m) {
    std::cout << kv.second << " threads participated in " << kv.first << std::endl;
  }
}

int main() {
  next_tid = 0;
  fig_11_5();
  dump_participation();
  return 0;
}

