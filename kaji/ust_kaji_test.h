#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER ust_kaji_test

#if !defined(_TRACEPOINT_UST_KAJI_TEST_H) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _TRACEPOINT_UST_KAJI_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lttng/tracepoint.h>

TRACEPOINT_EVENT(ust_kaji_test, tptest,
	TP_ARGS(),
	TP_FIELDS()
)

#endif /* _TRACEPOINT_UST_KAJI_TEST_H */

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "./ust_kaji_test.h"

/* This part must be outside ifdef protection */
#include <lttng/tracepoint-event.h>

#ifdef __cplusplus
}
#endif
