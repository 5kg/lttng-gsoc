#include <lttng/ust-events.h>
#include <lttng/tracepoint.h>
#include <stdlib.h>

/* A dummy probe */
void probe(void)
{
    printf("Nani????\n");
}

/*
 * The following data structs should be defined at top level,
 * since they will be accessed in destructor.
 */

/* Define a tracepoint */
struct tracepoint test_tracepoint = {
    .name = "test:testevent",
    .state = 0,
    .probes = NULL,
    .tracepoint_provider_ref = NULL,
    .signature = "testevent",
};

const struct tracepoint *test_tracepoints[] = { &test_tracepoint };

/* Define a tracepoint event */
struct lttng_event_desc test_event_desc = {
    .name = "test:testevent",
    .probe_callback = probe,
    .ctx = NULL,
    .fields = NULL,
    .nr_fields = 0,
    .signature = "testevent",
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

void __tracepoint_cb(const struct tracepoint* __tracepoint)
{
    struct tracepoint_probe *__tp_probe;

    __tp_probe = __tracepoint->probes;
    do {
        void (*__tp_cb)(void) = __tp_probe->func;
        void *__tp_data = __tp_probe->data;
        URCU_FORCE_CAST(void (*)(), __tp_cb)();
    } while ((++__tp_probe)->func);
}

void tracepoint_of(const struct tracepoint* __tracepoint)
{
    if (caa_unlikely(__tracepoint->state))
        __tracepoint_cb(__tracepoint);
}

int main()
{
    tracepoint_register_lib(&test_tracepoints, 1);
    /* Do the registration */
    lttng_probe_register(&desc);

    tracepoint_of(test_tracepoints[0]);

    return 0;
}
