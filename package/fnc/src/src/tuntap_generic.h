#ifndef FNC_TUNTAP_GENERIC_H
#define FNC_TUNTAP_GENERIC_H

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ev.h>

#include "buffer.h"
#include "privsep.h"
#include "fnc.h"

enum tuntap_type {
    FNC_TUNTAPMODE_TUN,
    FNC_TUNTAPMODE_TAP
};

struct tuntap_s
{
    int fd;
    int maxmtu;
    char devname[FNC_IFNAMSIZ];
    enum tuntap_type type;
    circular_buffer_t *sbuf;
    ev_io io_read;
    ev_io io_write;
};

int fnc_tuntap_alloc(struct tuntap_s *tuntap);
int fnc_tuntap_read(struct tuntap_s *tuntap);
int fnc_tuntap_write(struct tuntap_s *tuntap);
int fnc_tuntap_generic_read(u_char *data, uint32_t len);

#endif
