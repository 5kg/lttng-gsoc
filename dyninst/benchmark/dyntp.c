#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "ust_tests_benchmark.h"

void tpbench(int v)
{
	tracepoint(ust_tests_benchmark, tpbench, v);
}

void tpbench_no_arg(void)
{
	tracepoint(ust_tests_benchmark, tpbench_no_arg);
}
