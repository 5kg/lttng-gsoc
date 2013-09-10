#include <lttng/ust-events.h>
#include <lttng/tracepoint.h>
#include <lttng/ringbuffer-config.h>
#include <stdlib.h>

/* The probe function */
void probe(struct lttng_event* __tp_data)
{
    struct lttng_event *__event = (struct lttng_event *) __tp_data;
    struct lttng_channel *__chan = __event->chan;
    struct lttng_ust_lib_ring_buffer_ctx __ctx;
    size_t __event_len, __event_align;
    int __ret;

    __event_len = 0;
    __event_align = 0;
    lib_ring_buffer_ctx_init(&__ctx, __chan->chan, __event, __event_len,
                 __event_align, -1, __chan->handle);
    __ctx.ip = __builtin_return_address(0);
    __ret = __chan->ops->event_reserve(&__ctx, __event->id);
    if (__ret < 0)
        return;
    __chan->ops->event_commit(&__ctx);
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
    .probe_callback = (void (*)()) probe,
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

/* Tracepoint callback, call the probe function */
void __tracepoint_cb(const struct tracepoint* __tracepoint)
{
    struct tracepoint_probe *__tp_probe;

    __tp_probe = __tracepoint->probes;
    do {
        void (*__tp_cb)(void) = __tp_probe->func;
        void *__tp_data = __tp_probe->data;
        URCU_FORCE_CAST(void (*)(struct lttng_event*), __tp_cb)(__tp_data);
    } while ((++__tp_probe)->func);
}

/* Runtime tracepoint */
void tracepoint_of(const struct tracepoint* __tracepoint)
{
    if (caa_unlikely(__tracepoint->state))
        __tracepoint_cb(__tracepoint);
}

int main()
{
    int i;

    /* Register tracepoint */
    tracepoint_register_lib(&test_tracepoints, 1);
    /* Register probe */
    lttng_probe_register(&desc);

    for (i = 0; i < 10; ++i)
        tracepoint_of(test_tracepoints[0]);

    return 0;
}
