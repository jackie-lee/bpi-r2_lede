#include <stdio.h>
#include <unistd.h>

#define BAUDRATE 115200
#define CMD_SIZE 256
#define IP_SIZE 20
#define OPTR_SIZE 24

int goatcon_check_module();
int goatcon_insmod(const char* path);
int goatcon_open_dev(unsigned char dev_num);
int goatcon_close_dev(int fd);
int goatcon_set_ATE(int fd);

typedef struct _SYS_INFO{
	unsigned char srv_status;
}SYS_INFO;

int goatcon_get_sysinfo(int fd, SYS_INFO *sysif);
int goatcon_check_PIN(int fd);
int goatcon_check_CGREG(int fd);

typedef struct _ops {
	char operator[OPTR_SIZE];
	unsigned char sys_mod;
}OPS;

int goatcon_get_ops(int fd, OPS *ops);
int goatcon_get_signal(int fd);
int goatcon_connect(int fd, OPS *ops);
int goatcon_reset(int fd);
int goatcon_connect_status(int fd);
int goatcon_get_TXRX(const char *dev_name, unsigned long long *txB, unsigned long long *rxB);
int goatcon_get_MAC(const char *dev_name, char *mac);
int goatcon_get_IP(const char *dev_name, char *addr, char *gateway, char *mask);
int goatcon_set_net(int fd, OPS *ops, const char *apn, const char *user, const char* pwd);
int goatcon_check_addr(int fd);