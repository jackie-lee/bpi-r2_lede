#ifndef FNC_TIMESTAMP_H
#define FNC_TIMESTAMP_H

#include <stdint.h>
#include <ev.h>


uint64_t
fnc_timestamp64(ev_tstamp now);

uint16_t
fnc_timestamp16(uint64_t now);

uint16_t
fnc_timestamp16_diff(uint16_t tsnew, uint16_t tsold);

#endif