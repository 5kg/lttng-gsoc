#include <lttng/ust-events.h>
#include <stdlib.h>

/* A dummy probe */
void probe(void)
{
}

/*
 * The following data structs should be defined at top level,
 * since they will be accessed in destructor.
 */

/* Define a tracepoint event */
struct lttng_event_desc test_event_desc = {
    .name = "test:testevent",
    .probe_callback = probe,
    .ctx = NULL,
    .fields = NULL,
    .nr_fields = 0,
    .signature = "testeveet",
};

const struct lttng_event_desc *event_desc[] = { &test_event_desc };

/* Define a tracepoint probe */
struct lttng_probe_desc desc = {
    .provider = "test",
    .event_desc = event_desc,
    .nr_events = 1,
    .head = { NULL, NULL },
    .lazy_init_head = { NULL, NULL },
    .lazy = 0,
    .major = LTTNG_UST_PROVIDER_MAJOR,
    .minor = LTTNG_UST_PROVIDER_MINOR,
};

int main()
{
    /* Do the registration */
    lttng_probe_register(&desc);

    return 0;
}
