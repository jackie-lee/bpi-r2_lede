#ifndef FNC_CONTROL_H
#define FNC_CONTROL_H

#include <ev.h>

#define FNC_CTRL_EOF 0x04
#define FNC_CTRL_TERMINATOR '\n'
/* Control socket (mkfifo and/or AF_INET6?) */
#define FNC_CTRL_BUFSIZ 1024
/* Control timeout in seconds */
#define FNC_CTRL_TIMEOUT 5
struct fnc_control
{
    int mode;
    /* TODO: PATHMAX */
    char fifo_path[1024];
    mode_t fifo_mode;
    int fifofd;
    char *bindaddr;
    char *bindport;
    int sockfd;
    /* Client part */
    int clientfd; /* Only supports one client for now */
    time_t last_activity;
    char rbuf[FNC_CTRL_BUFSIZ];
    int rbufpos;
    char *wbuf;
    int wbuflen;
    int wbufpos;
    int http; /* HTTP mode ? 1 for inet socket */
    int close_after_write;
    ev_io fifo_watcher;
    ev_io sock_watcher;
    ev_io client_io_read;
    ev_io client_io_write;
    ev_timer timeout_watcher;
};

enum {
    FNC_CONTROL_DISABLED,
    FNC_CONTROL_READONLY,
    FNC_CONTROL_READWRITE
};

void
fnc_control_init(struct fnc_control *ctrl);

int
fnc_control_accept(struct fnc_control *ctrl, int fd);

int
fnc_control_timeout(struct fnc_control *ctrl);

void
fnc_control_parse(struct fnc_control *ctrl, char *line);

int
fnc_control_read_check(struct fnc_control *ctrl);

/* inside control, write to buffer */
int
fnc_control_write(struct fnc_control *ctrl, void *buf, size_t len);

/* From main loop */
int
fnc_control_read(struct fnc_control *ctrl);

/* From main loop */
int
fnc_control_send(struct fnc_control *ctrl);

#endif
