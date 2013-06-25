#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "ust_test.h"

void tpbench(int* v)
{
	tracepoint(ust_test, tptest, *v);
}
