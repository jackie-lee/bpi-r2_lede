#include "fnc.h"

extern struct fnc_filters_s fnc_filters;

fnc_tunnel_t *
fnc_filters_choose(uint32_t pktlen, const u_char *pktdata) {
    int i;
    struct pcap_pkthdr hdr;
    fnc_tunnel_t *tun;
    memset(&hdr, 0, sizeof(hdr));
    hdr.caplen = pktlen;
    hdr.len = pktlen;
    for(i = 0; i < fnc_filters.count; i++) {
        tun = fnc_filters.tun[i];
        /* Don't even consider offline interfaces */
        /* log_debug("filters", "check filter[%d] (%s)", i, tun->name); */
        if (pcap_offline_filter(&fnc_filters.filter[i], &hdr, pktdata) != 0) {
            if (tun->status < FNC_AUTHOK) {
                /* log_debug("filters", "tun %s is offline.", tun->name); */
                continue;
            }
            return tun;
        }
    }
    return NULL;
}

int
fnc_filters_add(const struct bpf_program *filter, fnc_tunnel_t *tun) {
    if (fnc_filters.count >= 255) {
        return -1;
    }
    memcpy(&fnc_filters.filter[fnc_filters.count], filter, sizeof(*filter));
    fnc_filters.tun[fnc_filters.count] = tun;
    fnc_filters.count++;
    return 0;
}
