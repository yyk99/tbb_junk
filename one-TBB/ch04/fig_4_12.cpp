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
#include <vector>

#include <tbb/tbb.h>
#include <oneapi/dpl/execution>
#include <oneapi/dpl/algorithm>
#include <oneapi/dpl/numeric>

//
// For best performance when using the Intel compiler use
// EXTRA_CXXFLAGS="-qopenmp-simd -xHost" when building

const int num_versions = 5;

static void warmupTBB();
void accumulateTime(tbb::tick_count& t0, int version);
void dumpTimes();

void fig_4_12_a() {
  const int num_trials = 1000;
  const int n = 65536;
  tbb::tick_count t0;

  std::vector<float> a(n, 1.0), b(n, 1.0);

  for (int t = 0; t < num_trials; ++t) {
    warmupTBB();
    t0 = tbb::tick_count::now();
    auto v = std::transform_reduce(dpl::execution::par,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f,
      /* op1, the reduce */
      [](float ae, float be) -> float {
        return ae + be;
      },
      /* op2, the transform */
      [](float ae, float be) -> float {
        return ae * be;
      }
    );
    accumulateTime(t0, 3);
    v += std::transform_reduce(dpl::execution::par_unseq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f,
      /* op1, the reduce */
      [](float ae, float be) -> float {
        return ae + be;
      },
      /* op2, the transform */
      [](float ae, float be) -> float {
        return ae * be;
      }
    );
    accumulateTime(t0, 4);
#pragma novector
    for (int i = 0; i < n; ++i) {
      v += a[i]*b[i];
    }
    accumulateTime(t0, 0);
    v += std::transform_reduce(dpl::execution::seq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f,
      /* op1, the reduce */
      [](float ae, float be) -> float {
        return ae + be;
      },
      /* op2, the transform */
      [](float ae, float be) -> float {
        return ae * be;
      }
    );
    accumulateTime(t0, 1);
    v += std::transform_reduce(dpl::execution::unseq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f,
      /* op1, the reduce */
      [](float ae, float be) -> float {
        return ae + be;
      },
      /* op2, the transform */
      [](float ae, float be) -> float {
        return ae * be;
      }
    );
    accumulateTime(t0, 2);
    if (v != num_versions * n) 
      std::cout << "ERROR: value is not correct" 
                << v << " != " << num_versions * n << std::endl;
  }
  dumpTimes();
}

void fig_4_12_b() {
  const int num_trials = 1000;
  const int n = 65536;
  tbb::tick_count t0;

  std::vector<float> a(n, 1.0), b(n, 1.0);

  for (int t = 0; t < num_trials; ++t) {
    warmupTBB();
    t0 = tbb::tick_count::now();
    auto v = std::transform_reduce(dpl::execution::par,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f
    );
    accumulateTime(t0, 3);
    v += std::transform_reduce(dpl::execution::par_unseq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f
    );
    accumulateTime(t0, 4);
#pragma novector
    for (int i = 0; i < n; ++i) {
      v += a[i]*b[i];
    }
    accumulateTime(t0, 0);
    v += std::transform_reduce(dpl::execution::seq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f
    );
    accumulateTime(t0, 1);
    v += std::transform_reduce(dpl::execution::unseq,
      /* in1 range */ a.begin(), a.end(),
      /* in2 range */ b.begin(),
      /* init */ 0.0f
    );
    accumulateTime(t0, 2);
    if (v != num_versions * n) 
      std::cout << "ERROR: value is not correct" 
                << v << " != " << num_versions * n << std::endl;
  }
  dumpTimes();
}

double total_times[num_versions] = {0,0,0,0,0};

void accumulateTime(tbb::tick_count& t0, int version) {
  if (version >= 0) {
    double elapsed_time = (tbb::tick_count::now()-t0).seconds(); 
    total_times[version] += elapsed_time;
    t0 = tbb::tick_count::now();
  }
}

void dumpTimes() {
  const char *versions[num_versions] = 
    { "for", "seq", "unseq", "par", "par_unseq" }; 
 
  for (int i = 0; i < num_versions; ++i) {
    std::cout << versions[i] << ", " << total_times[i] << std::endl;
    total_times[i] = 0.0;
  }
}

static void warmupTBB() {
  tbb::parallel_for(0, tbb::this_task_arena::max_concurrency(), [](int) {
    tbb::tick_count t0 = tbb::tick_count::now();
    while ((tbb::tick_count::now() - t0).seconds() < 0.01);
  });
}

int main() {
  double  total_time = 0.0;
  {
    tbb::tick_count t0 = tbb::tick_count::now();
    fig_4_12_a(); 
    fig_4_12_b(); 
    total_time = (tbb::tick_count::now() - t0).seconds();
  }
  std::cout << "total_time == " << total_time << " seconds" << std::endl;
  return 0;
}

