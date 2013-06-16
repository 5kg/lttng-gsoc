#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "ust_test.h"

void tptest(void)
{
	tracepoint(ust_test, tptest);
}
