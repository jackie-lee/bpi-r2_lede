#ifndef _FNC_BUFFER_H
#define _FNC_BUFFER_H

#include <sys/types.h>
#include <sys/queue.h>
#include "pkt.h"

/* Basic circular buffer
 * data can be stored inside the struct, but that's not mandatory.
 * data is not used directly by fnc_cb_*.
 */
typedef struct
{
    int size;
    int start;
    int end;
    void *data;
} circular_buffer_t;

typedef struct
{
    fnc_pkt_t **pkts;
} pktbuffer_t;


struct pkt_entry {
    fnc_pkt_t pkt;
    TAILQ_ENTRY(pkt_entry) entries;
};

typedef struct {
    uint32_t size;
    uint32_t used;
    TAILQ_HEAD(, pkt_entry) free_head;
    TAILQ_HEAD(, pkt_entry) used_head;
} freebuffer_t;

/**
 * Generic circular buffer handling
 */

/* Allocate a circular of size +1 element of sizememb length */
circular_buffer_t *
fnc_cb_init(int size);

void
fnc_cb_free(circular_buffer_t *buf);

void
fnc_cb_reset(circular_buffer_t *buf);

int
fnc_cb_is_full(const circular_buffer_t *buf);

int
fnc_cb_is_empty(const circular_buffer_t *buf);

void *
fnc_cb_read(circular_buffer_t *buf, void **data);

void *
fnc_cb_read_norelease(const circular_buffer_t *buf, void **data);

void *
fnc_cb_write(circular_buffer_t *buf, void **data);

/**
 * Application specific cirtular buffer handlers
 */

circular_buffer_t *
fnc_pktbuffer_init(int size);

void
fnc_pktbuffer_free(circular_buffer_t *buf);

void
fnc_pktbuffer_reset(circular_buffer_t *buf);

fnc_pkt_t *
fnc_pktbuffer_read(circular_buffer_t *buf);

fnc_pkt_t *
fnc_pktbuffer_read_norelease(circular_buffer_t *buf);

fnc_pkt_t *
fnc_pktbuffer_write(circular_buffer_t *buf);


/**
 * Single allocation buffers (used for reordering)
 */
freebuffer_t *
fnc_freebuffer_init(uint32_t size);

void
fnc_freebuffer_reset(freebuffer_t *freebuf);

fnc_pkt_t *
fnc_freebuffer_get(freebuffer_t *freebuf);

void
fnc_freebuffer_free(freebuffer_t *freebuf, fnc_pkt_t *pkt);

fnc_pkt_t *
fnc_freebuffer_drain_used(freebuffer_t *freebuf);

#endif
