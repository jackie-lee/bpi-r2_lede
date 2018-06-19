#include "fnc.h"

int
fnc_tuntap_generic_read(u_char *data, uint32_t len)
{
    circular_buffer_t *sbuf;
    fnc_tunnel_t *rtun = NULL;
    fnc_pkt_t *pkt;

#ifdef HAVE_FILTERS
    rtun = fnc_filters_choose((uint32_t)len, data);
    if (rtun) {
        /* High priority buffer, not reorderd when a filter applies */
        sbuf = rtun->hpsbuf;
    }
#endif
    if (!rtun) {
        rtun = fnc_rtun_choose();
        /* Not connected to anyone. read and discard packet. */
        if (! rtun) 
            return len;
        sbuf = rtun->sbuf;
    }
    if (fnc_cb_is_full(sbuf))
        log_warnx("tuntap", "%s buffer: overflow", rtun->name);

    /* Ask for a free buffer */
    pkt = fnc_pktbuffer_write(sbuf);
    pkt->len = len;
    /* TODO: INEFFICIENT COPY */
    memcpy(pkt->data, data, pkt->len);
    if (!ev_is_active(&rtun->io_write) && !fnc_cb_is_empty(sbuf)) {
        ev_io_start(EV_DEFAULT_UC, &rtun->io_write);
    }
    return pkt->len;
}
