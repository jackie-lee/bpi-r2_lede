#include "serial.h"
#include "ping.h"
#include "at.h"
	
struct timeval timeout;

int goatcon_check_module()
{
	//int r = system("lsmod |grep gobinet");
	FILE *r_stream;	
	char r_buf[RECV_SIZE];
	char cmd_str[CMD_SIZE];		
	r_stream = popen("lsmod |grep gobinet", "r");
	if (r_stream)
	{
		memset(r_buf, 0, RECV_SIZE);		
		fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
		if (NULL == strstr(r_buf, "gobinet"))
		{
			printf("4G driver has not been install\n");
			return -1;
		}

		pclose(r_stream);
	}

	return 0;
}

int goatcon_insmod(const char* path)
{
	FILE *r_stream;
	char r_buf[RECV_SIZE];
	char cmd_str[CMD_SIZE];
	if (0 == access(path, F_OK))
	{
		memset(cmd_str, 0, CMD_SIZE);
		sprintf(cmd_str, "insmod %s", path);
        	r_stream = popen(cmd_str, "r");
	        if (r_stream)
	        {
        	        memset(r_buf, 0, RECV_SIZE);
	                fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
        	        if (0 != strlen(r_buf))
                	{
                        	printf("4G driver install fail\n");
	                        return -2;
        	        }

                	pclose(r_stream);
	        }		
	}
	else
	{
		printf("module file %s not exist, errno:%d\n", path, errno);
		return -1;
	}

	return 0;
}

int goatcon_open_dev(unsigned char dev_num)
{
	char recv[RECV_SIZE];
	char openusb[14] = {0};
	sprintf(openusb, "%s%d", "/dev/ttyUSB", dev_num);
	printf("%s\n", openusb);
	int fd = at_open_serial(openusb);
	if (fd < 0)
	{
	        printf("open %s fail\n", openusb);
	        return -1;
	}
	else
	{
		timeout.tv_sec=1;
		timeout.tv_usec=0;	
		at_set_serial(fd, BAUDRATE, 8, -1, 1);
	}

	return fd;
}

int goatcon_close_dev(int fd)
{
	at_close_serial(fd);
	return 0;
}

int goatcon_set_ATE(int fd)
{
	char recv[RECV_SIZE];
	at_write_serial(fd,"ATE\r\n",5);
	memset(recv, 0, RECV_SIZE);
	at_read_serial(fd, recv, &timeout);
	printf("%s\n", recv);
	if(strstr(recv, "OK")==NULL){
		printf("at command ate fail\n");
		return -1;
	}
	
	return 0;
}
/*
typedef struct _SYS_INFO{
	unsigned char srv_status;
}SYS_INFO;
*/
int goatcon_get_sysinfo(int fd, SYS_INFO *sysif)
{
	char recv[RECV_SIZE];
	at_write_serial(fd, "AT^SYSINFO\r\n", 12);  
	memset(recv, 0, RECV_SIZE);
	at_read_serial(fd, recv, &timeout);
	printf("%s\n", recv);
	if (strstr(recv,"^SYSINFO:") == NULL){
		printf("command sysinfo fail\n");
		return -1;
	}
	char *pD = strstr(recv,"^SYSINFO:");
	pD = pD + 9;
	char buf[4];
	memset(buf, 0, 4);
	sscanf(pD, "%[^,]", buf);
	sysif->srv_status = atoi(buf);
	printf("status=%d\n", sysif->srv_status);
	
	return 0;
}

// ready or not
int goatcon_check_PIN(int fd)
{
	char recv[RECV_SIZE];
	at_write_serial(fd, "AT+CPIN?\r\n", 10);
	memset(recv, 0, RECV_SIZE);
	at_read_serial(fd, recv, &timeout);
	printf("%s\n", recv);
	if (strstr(recv, "READY") == NULL){
		printf("command at+cpin? fail\n");

		if (strstr(recv, "SIM failure") != NULL)
		{
			return -1;
		}
		else
		{
			return -2;
		}
        }

	return 0;
}

int goatcon_check_CGREG(int fd)
{
	char recv[RECV_SIZE];
        at_write_serial(fd, "AT+CGREG?\r\n", 11);
        memset(recv, 0, RECV_SIZE);
        at_read_serial(fd, recv, &timeout);
        printf("%s\n", recv);
        char *yu;
	char buf[4];
        char buf1[4];
        memset(buf, 0, 4);
        memset(buf1, 0, 4);
        yu = strstr(recv, "+CGREG:");
        yu = yu + 7;
        sscanf(yu, "%[^,],%s", buf, buf1);
        int y = atoi(buf1);
        if ( y != 1) {
		printf("cgreg register fail\n");
		return -1;
        }

	return 0;
}
/*
typedef struct _ops {
	char operator[24];
	unsigned char sys_mod;
}OPS;
*/
int goatcon_get_ops(int fd, OPS *ops)
{
	char recv[RECV_SIZE];
	at_write_serial(fd, "AT+COPS=0,2\r\n", 13);
	memset(recv, 0, RECV_SIZE);
	at_read_serial(fd, recv, &timeout);
	printf("%s\n", recv);
	at_write_serial(fd, "\r\n", 2);
	memset(recv, 0, RECV_SIZE);
	at_read_serial(fd, recv, &timeout);
	printf("%s\n", recv);	
	at_write_serial(fd, "AT+COPS?\r\n", 10);
	memset(recv, 0, RECV_SIZE);
	at_read_serial(fd, recv, &timeout);
	printf("%s\n", recv);
	char buf[4];
	char buf2[OPTR_SIZE];
	char *pCOPS, *pCOPS1 = NULL;
	if (strstr(recv, "+COPS:") != NULL){
		memset(buf, 0, 4);
		memset(buf2, 0, OPTR_SIZE);
		pCOPS = strchr(recv, '\"');
		printf("%s\n", pCOPS);
		pCOPS1 = strchr(pCOPS + 1, '\"');
		printf("%s\n", pCOPS1);
		strncpy(buf2, pCOPS + 1, pCOPS1 - pCOPS - 1);
		printf("%s\n", buf2);
		//sscanf(pCOPS, "%[^,],%s", buf2, buf);
		pCOPS = strchr(pCOPS1, ',');
		strncpy(buf, pCOPS + 1, 1);
		memset(ops->operator, 0, OPTR_SIZE);
		strcpy(ops->operator, buf2);
		ops->sys_mod = atoi(buf);
	}
	else
	{
		printf("Read operator fail\n");
		return -1;
	}

	return 0;
}

int goatcon_get_signal(int fd)
{
	char recv[RECV_SIZE];
	int signal = 0;
        at_write_serial(fd, "AT+CSQ\r\n", 8);
        memset(recv, 0, RECV_SIZE);
        at_read_serial(fd, recv, &timeout);
        printf("%s\n", recv);
        if(strstr(recv, "+CSQ:") == NULL){
                printf("command at csq fail \n");
		return -1;
        }
	 char buf[4];
        memset(buf, 0, 4);
        char *p = strstr(recv,"+CSQ:");
        p = p + 5;
        sscanf(p,"%[^,]",buf);
        signal = atoi(buf);
        printf("signal =%d\n",signal);
	return signal;
}

int goatcon_set_net(int fd, OPS *ops, const char *apn, const char *user, const char* pwd)
{
	char recv[RECV_SIZE];
	char cmd[128] = {0};
        if(strstr(ops->operator,"CHN-CT")!=NULL){
                if(ops->sys_mod==0||ops->sys_mod==2){
                        sprintf(cmd, "AT^NETCFG=0,32774,,,4,,,\"\",0,\"%s\",\"%s\",1\r\n", user, pwd);
                }
                else{
                        sprintf(cmd, "AT^NETCFG=0,32774,,,4,,,\"\",0,\"%s\",\"%s\",0\r\n", user, pwd);
                }
        }
        else if (strstr(ops->operator, "CHINA TELECOM") != NULL)
        {
                if(ops->sys_mod == 0||ops->sys_mod == 2 ||ops->sys_mod == 8){
			sprintf(cmd, "AT^NETCFG=0,32774,,,4,,,\"\",0,\"%s\",\"%s\",1\r\n", user, pwd);
                }
                else{
                        sprintf(cmd, "AT^NETCFG=0,32774,,,4,,,\"\",0,\"%s\",\"%s\",0\r\n", user, pwd);
                }
        }
        else{
                sprintf(cmd, "AT^NETCFG=0,32774,,,4,,,\"\",0,\"%s\",\"%s\",0\r\n", user, pwd);

        }	
        at_write_serial(fd,cmd,51);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
        if(strstr(recv,"OK")==NULL){
            printf("AT^NETCFG fail\n");
		return -1;
         }

	memset(cmd, 0, 128);
	sprintf(cmd, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", apn);
        at_write_serial(fd,cmd,51);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
        if(strstr(recv,"OK")==NULL){
            printf("AT+CGDCONT fail\n");
		return -2;
         }		
	
	return 0;
}

int goatcon_connect(int fd, OPS *ops)
{
	char recv[RECV_SIZE];
        char *atcmd2="AT$QCRMCALL=1,1,1,2,1\r\n";
        char *atcmd3="AT$QCRMCALL=1,1,1,1,1\r\n";

	char *qcrmcall = NULL;
        if(strstr(ops->operator,"CHN-CT")!=NULL){
                if(ops->sys_mod==0||ops->sys_mod==2){
                        qcrmcall=atcmd3;
                }
                else{
                        qcrmcall=atcmd2;
                }
        }
        else if (strstr(ops->operator, "CHINA TELECOM") != NULL)
        {
                if(ops->sys_mod == 0||ops->sys_mod == 2 ||ops->sys_mod == 8){
                        qcrmcall=atcmd3;
                        qcrmcall=atcmd3;
                }
                else{
                        qcrmcall=atcmd2;
                }
        }
        else{
                qcrmcall=atcmd2;

        }
        printf("qcrmcall=%s\n",qcrmcall);

        at_write_serial(fd,qcrmcall,23);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
        if(strstr(recv,"$QCRMCALL:") == NULL){
            printf("AT$QCRMCALL fail\n");
		return -1;
         }

	return 0;
}

int goatcon_reset(int fd)
{
	at_write_serial(fd,"AT+CFUN=1,1\r\n",13);
	return 0;
}

int goatcon_disconnect(int fd)
{
	at_write_serial(fd, "AT$QCRMCALL=0,1\r\n", 17);
	return 0;
}

int goatcon_connect_status(int fd)
{
	char recv[RECV_SIZE];
	char buf[4];
	char buf2[10];
	int con = -1;
        at_write_serial(fd,"AT$QCRMCALL?\r\n",14);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
        if(strstr(recv,"$QCRMCALL:")==NULL){
            printf("RECV=%s\n",recv);
            printf("QCRMCALL check fail\n");
            return 0;
         }
         else{
            memset(buf,0,4);
            memset(buf2,0,10);
            char *pCH=strstr(recv,"$QCRMCALL:");
            pCH=pCH+10;
            sscanf(pCH,"%[^,],%s",buf2,buf);
            con = atoi(buf2);
            printf("connect=%d\n",con); 
            printf("protocal=%s\n",buf);
        }

	return con;
}

/*
root@LEDE:~# ifconfig usb0 |awk '/RX bytes:\d+ \(\d+.\d KiB\)  TX bytes:\d+ \(\d+.\d KiB\)/{print $3,$4,$7,$8}'
(570.5 KiB) (635.2 KiB)

root@LEDE:~# ifconfig br-lan |grep "TX bytes:"|awk -F "TX bytes:" '{print $2}'|awk -F "\(" '{print $1}'
2445876
*/
int goatcon_get_TXRX(const char *dev_name, unsigned long long *txB, unsigned long long *rxB)
{	
	FILE *r_stream;	
	char r_buf[RECV_SIZE];
	char cmd_str[CMD_SIZE];	
	memset(cmd_str, 0, CMD_SIZE);
	char *endptr;	
	sprintf(cmd_str, "ifconfig %s |grep \"TX bytes:\"|awk -F \"TX bytes:\" '{print $2}'|awk -F \"\(\" '{print $1}'",dev_name);
	r_stream = popen(cmd_str, "r");
	if (r_stream)
	{
		memset(r_buf, 0, RECV_SIZE);		
		fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
		if (strlen(r_buf) > 0)		
			*txB = strtoull(r_buf, &endptr, 10);

		pclose(r_stream);
	}

	memset(cmd_str, 0, CMD_SIZE);
	sprintf(cmd_str, "ifconfig %s |grep \"RX bytes:\"|awk -F \"RX bytes:\" '{print $2}'|awk -F \"\(\" '{print $1}'", dev_name);
	r_stream = popen(cmd_str, "r");
	if (r_stream)
	{
		memset(r_buf, 0, RECV_SIZE);		
		fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
		if (strlen(r_buf) > 0)		
			*rxB = strtoull(r_buf, &endptr, 10);

		pclose(r_stream);
	}
	
	return 0;
}

/*
root@LEDE:~# ifconfig usb0 |grep HWaddr |awk -F "HWaddr " '{print $2}'
CA:BA:57:F4:43:D2
*/
int goatcon_get_MAC(const char *dev_name, char *mac)
{
	FILE *r_stream;	
	char *pch = NULL;
	char r_buf[RECV_SIZE];
	char cmd_str[CMD_SIZE];	
	memset(cmd_str, 0, CMD_SIZE);
	sprintf(cmd_str, "ifconfig %s |grep HWaddr |awk -F \"HWaddr \" '{print $2}'", dev_name);
	printf("%s\n", cmd_str);
	r_stream = popen(cmd_str, "r");
	if (r_stream)
	{
		memset(r_buf, 0, RECV_SIZE);		
		fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
		printf("%s\n", r_buf);
		pch = strchr(r_buf, '\n');
		if (pch)
			*(pch - 1) = '\0';		
		if (strlen(r_buf) > 0)
			strcpy(mac, r_buf);
		pclose(r_stream);
	}
	
	return 0;
}

/*
root@LEDE:~# ifconfig br-lan |grep Mask
          inet addr:192.168.1.1  Bcast:192.168.1.255  Mask:255.255.255.0
try regex in sscanf

root@LEDE:~# ifconfig br-lan |grep Mask |awk -F "addr:" '{print $2}'|awk -F " Bcast:" '{print $1}'
192.168.1.1 

root@LEDE:~# ifconfig br-lan |grep Mask |awk -F "Bcast:" '{print $2}'|awk -F " Mask:" '{print $1}'
192.168.1.255 

root@LEDE:~# ifconfig br-lan |grep Mask |awk -F "Mask:" '{print $2}'
255.255.255.0
*/
int goatcon_get_IP(const char *dev_name, char *addr, char *gateway, char *mask)
{
	FILE *r_stream;		
	char *pch = NULL;
	char r_buf[RECV_SIZE];
	char cmd_str[CMD_SIZE];	
	memset(cmd_str, 0, CMD_SIZE);
	sprintf(cmd_str, "ifconfig %s |grep Mask |awk -F \"addr:\" '{print $2}'|awk -F \" Bcast:\" '{print $1}'", dev_name);	
	r_stream = popen(cmd_str, "r");
	if (r_stream)
	{
		memset(r_buf, 0, RECV_SIZE);		
		fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
		pch = strchr(r_buf, '\n');
		if (pch)
			*(pch - 1) = '\0';
		if (strlen(r_buf) > 0)
		strcpy(addr, r_buf);
		pclose(r_stream);
	}

	memset(cmd_str, 0, CMD_SIZE);
	sprintf(cmd_str, "ifconfig %s |grep Mask |awk -F \"Bcast:\" '{print $2}'|awk -F \" Mask:\" '{print $1}'", dev_name);	
	r_stream = popen(cmd_str, "r");
	if (r_stream)
	{
		memset(r_buf, 0, RECV_SIZE);		
		fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
		pch = strchr(r_buf, '\n');
		if (pch)
			*(pch - 1) = '\0';
		if (strlen(r_buf) > 0)
			strcpy(gateway, r_buf);
		pclose(r_stream);
	}

	memset(cmd_str, 0, CMD_SIZE);
	sprintf(cmd_str, "ifconfig %s |grep Mask |awk -F \"Mask:\" '{print $2}'", dev_name);	
	r_stream = popen(cmd_str, "r");
	if (r_stream)
	{
		memset(r_buf, 0, RECV_SIZE);		
		fread(r_buf, sizeof(char), RECV_SIZE, r_stream);
		pch = strchr(r_buf, '\n');
		if (pch)
			*(pch - 1) = '\0';
		if (strlen(r_buf) > 0)
			strcpy(mask, r_buf);
		pclose(r_stream);
	}
	
	return 0;
}

int goatcon_check_addr(int fd)
{
	char recv[RECV_SIZE];
	at_write_serial(fd, "AT+CGPADDR\r\n", 12);
	memset(recv, 0, RECV_SIZE);
	at_read_serial(fd, recv, &timeout);
	printf("%s\n", recv);
	if (strstr(recv, "OK") != NULL)
	{
		if (strstr(recv, "+CGPADDR:1,0.0.0.0") != NULL)
		{
			printf("goatcon get ip addr fail\n");
			return -2;	
		}
	}
	else
	{
		printf("command at+cgpaddr fail\n");
		return -1;			
	}

	printf("goatcon get ip addr ok\n");
	return 0;
}

#if 1
int main(int argc, char *argv[])
{
	int usb_num = -1;
	char usb_if[8];
	char *apn = NULL;
	char *user = NULL;
	char *pwd = NULL;
	char cmd_str[CMD_SIZE];
	if (argc > 1)
	{
		usb_num = atoi(argv[1]);
		sprintf(usb_if, "usb%d", usb_num);

		if (argc > 2 )
		{
			apn = argv[2];
		}

		if (argc > 3)
		{
			user = argv[3];
		}

		if (argc > 4)
		{
			pwd = argv[4];
		}
	}
	else
	{
		usb_num = 0;
		sprintf(usb_if, "usb%d", 0);
	}
	
	if (0 != goatcon_check_module())
	{
		return goatcon_insmod("/lib/modules/4.9.44/gobinet.ko");
	}

	int fd = goatcon_open_dev(usb_num);
	if ( fd < 0)
	{
		return -1;
	}

	int ret = goatcon_set_ATE(fd);
	if (0 != ret)
	{
		goatcon_close_dev(fd);
		return -2;
	}

	ret = goatcon_check_PIN(fd);
	if (0 != ret)
	{
		if ( -1 == ret)
		{
			goatcon_reset(fd);
		}
		goatcon_close_dev(fd);
		return -3;
	}

	char addr[20];
	char gateway[20];	
	char mask[20];
	
#if 1	
	ret = goatcon_connect_status(fd);
	if (1 == ret)
	{
		memset(addr, 0, IP_SIZE);
		memset(gateway, 0, IP_SIZE);
		memset(mask, 0, IP_SIZE);
		ret = goatcon_get_IP(usb_if, addr, gateway, mask);
		printf("addr:%s\n", addr);
		printf("gateway:%s\n", gateway);
		printf("mask:%s\n", mask);		
		//if (0 != ret)
		if (0 == strcmp(addr, ""))
		{
			memset(cmd_str, 0, CMD_SIZE);
			sprintf(cmd_str, "ifconfig %s down", usb_if);
			system(cmd_str);
			sleep(2);
			memset(cmd_str, 0, CMD_SIZE);
			sprintf(cmd_str, "ifconfig %s up", usb_if);			
			system(cmd_str);
			sleep(5);

			goatcon_get_IP(usb_if, addr, gateway, mask);
			if (0 == strcmp(addr, ""))
			{
				goatcon_reset(fd);
			}
		}
		
		goatcon_close_dev(fd);
		return 0;
	}
#endif

	ret = goatcon_check_CGREG(fd);
	if (0 != ret)
	{
		goatcon_close_dev(fd);	
		return -4;
	}

	ret = goatcon_get_signal(fd);
	if (ret < 15)
	{
		goatcon_close_dev(fd);
		printf("signal is too low\n");
		return -5;
	}

	OPS ops;
	memset(ops.operator, 0, OPTR_SIZE);
	ops.sys_mod;
	ret = goatcon_get_ops(fd, &ops);
	if (0 != ret)
	{
		goatcon_close_dev(fd);
		return -6;
	}

	if (argc > 2)
	{
		ret = goatcon_set_net(fd, &ops, apn, user, pwd);
		if ( 0 != ret)
		{
			goatcon_close_dev(fd);	
			return -8;
		}		
	}
	
	ret = goatcon_connect(fd, &ops);
	if ( 0 != ret)
	{
		goatcon_close_dev(fd);	
		return -7;
	}

	sleep(5);
	
	if ( 0 != goatcon_check_addr(fd))
	{
		goatcon_close_dev(fd);	
		return -8;
	}

#if 1
	memset(addr, 0, IP_SIZE);
	memset(gateway, 0, IP_SIZE);
	memset(mask, 0, IP_SIZE);
	ret = goatcon_get_IP(usb_if, addr, gateway, mask);
	//if (0 != ret)
	if (0 == strcmp(addr, ""))
	{
		memset(cmd_str, 0, CMD_SIZE);
		sprintf(cmd_str, "ifconfig %s down", usb_if);
		system(cmd_str);
		sleep(2);
		memset(cmd_str, 0, CMD_SIZE);
		sprintf(cmd_str, "ifconfig %s up", usb_if);			
		system(cmd_str);
	}		
#endif

	goatcon_close_dev(fd);
	
	return 0;
}

#else
int main(int argc, char *argv[])
{
    int errtime=0;
    int fd=-1;
    //int run=0;
    struct timeval timeout;
    timeout.tv_sec=1;
    timeout.tv_usec=0;
    int net =-1;
    //int usbserial=1;
    char recv[RECV_SIZE];
    char buf[4];
    char *scmd1="AT^NETCFG=0,";
    char *scmd2="32774";
    char *scmd3=",,,4,,,";
    char *scmd4="\"\",0,\"CARD\",\"CARD\",1";
    char *scmd5="\r\n";
    char atcmd[128];
    int usbserial;
    char openusb[14];
    sprintf(atcmd,"%s%s%s%s%s",scmd1,scmd2,scmd3,scmd4,scmd5);
/*
reconn:
	
	printf("++++ waiting for install gobinet ++++\n");
	int t=system("rmmod qmi_wwan > /dev/null 2>&1");
	sleep(2);
	if(t<0){
	    printf("t=%d\n",t);
	    printf("rmmod qmi_wwan fail\n");
	}
	
	printf("t=%d\n",t);
	t=system("insmod GobiNet.ko > /dev/null 2>&1");
	sleep(2);
	if(t<0){
	    printf("t=%d\n",t);
	    printf("insmod GobiNet fail\n");
	}
	t = system("ifconfig usb0 up > /dev/null 2>&1");
	sleep(2);
	if(t<0){
	    printf("t=%d\n",t);
	    printf("up usb0 fail\n");
	}

    if(errtime>10){
	printf("fail over 10 times,please reboot and check.........\n");
	printf("errtime=%d\n",errtime);
	at_close_serial(fd);
	return -1;
    }
*/
	if (argc > 1)
	{
		usbserial = atoi(argv[1]);	
	}
	else
	{
		usbserial = 0;
	}

	if (fd < 0) {
		sprintf(openusb,"%s%d","/dev/ttyUSB",usbserial);
		printf("%s\n",openusb);
		fd=at_open_serial(openusb);	
		if (fd < 0)
		{
			printf("open %s fail\n", openusb);
			exit(1);
		}
/*		
		else
		{
			at_set_serial(fd,BAUDRATE,8,-1,1);

		        at_write_serial(fd,"AT+CFUN=1,1\r\n",13);
			at_close_serial(fd);

			sleep(30);
			fd = at_open_serial(openusb);
			
			if (fd < 0)
			{
				printf("open %s fail\n", openusb);
				exit(1);
			}
		}
*/		
	}

reconn:
	
	if (errtime > 10)
	{
		if (fd > 0)
		{
			at_close_serial(fd);
			exit(2);
		}
	}
//	fd=at_open_serial("/dev/ttyUSB0");
//	usbserial=0;
#if 0
//interrupt change USBserial, normal may not use this function
	while(fd<0){
		
	    //usbserial
	    //openusb[12]=usbserial;
	    sprintf(openusb,"%s%d","/dev/ttyUSB",usbserial);
	    printf("%s\n",openusb);
	    fd=at_open_serial(openusb);
	    //printf("%s\n",openusb);
	   
	    usbserial=usbserial+1;
	    if(usbserial>30){
		printf("too long USB test serial, please reboot module\n");
		return -1;
	    }
	}

#endif

        at_write_serial(fd,"ATE\r\n",5);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
	if(strstr(recv,"OK")==NULL){
		printf("at command ate fail\n");
		++errtime;
		sleep(10);
		goto reconn;
	}
        at_write_serial(fd,"ATI\r\n",5);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
	if(strstr(recv,"ESN:")==NULL){
	if(strstr(recv,"IMEI:")==NULL){		
		printf("at command ati fail\n");
		++errtime;
		sleep(10);
		goto reconn;
		}
	}	
        at_write_serial(fd,"AT+APPVER\r\n",11);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
	if(strstr(recv,"APP_VERSION:")==NULL){
		printf("at command appver fail\n");
		++errtime;
		sleep(10);
		goto reconn;
	}	
#if 1
        at_write_serial(fd,"AT^SYSINFO\r\n",12);  
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
        if(strstr(recv,"^SYSINFO:")==NULL){
                printf("command sysinfo fail\n");
		++errtime;
                sleep(10);
		goto reconn;
        }       
	char *pD =strstr(recv,"^SYSINFO:");
	pD=pD+9;
	memset(buf,0,4);
	sscanf(pD,"%[^,]",buf);
//	printf("pd=%s\n",pD+1);
	int status = atoi(buf);
	printf("status=%d\n",status);
	if(status!=2){
		printf("command sysinfo status fail\n");
		++errtime;
		sleep(10);
		goto reconn;	
	}
	
 #endif  
#if 1
	at_write_serial(fd,"AT+CPIN?\r\n",10);
	memset(recv,0,RECV_SIZE);
	at_read_serial(fd,recv,&timeout);
	printf("%s\n",recv);
	if(strstr(recv,"READY")==NULL){
		printf("command at+cpin? fail\n");
		++errtime;
		sleep(10);
		goto reconn;
	}	
 #endif   
	at_write_serial(fd,"AT+CGREG?\r\n",11);  
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
	char *yu;
	char buf1[4];
	memset(buf,0,4);
	memset(buf1,0,4);
	yu = strstr(recv,"+CGREG:");
	yu=yu+7;
	sscanf(yu,"%[^,],%s",buf,buf1);
	int y = atoi(buf1);
	//printf("cgreg =%d\n",y);
	if(y!=1){
	    printf("cgreg register fail\n");
	    errtime++;
	    sleep(10);
	    goto reconn;	
	}

	at_write_serial(fd,"AT+CREG?\r\n",10);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
        printf("%s\n",recv);
	memset(buf,0,4);
	memset(buf1,0,4);
	yu = strstr(recv,"+CREG:");
	yu=yu+6;
	sscanf(yu,"%[^,],%s",buf,buf1);
	y = atoi(buf1);
	if(y!=1){
	    printf("creg register fail\n");
	    errtime++;
	    sleep(10);
	    goto reconn;	
	}

        at_write_serial(fd,"AT+CSQ\r\n",8);
        memset(recv,0,RECV_SIZE);
        at_read_serial(fd,recv,&timeout);
	printf("%s\n",recv);
	if(strstr(recv,"+CSQ:")==NULL){
		printf("command at csq fail \n");
		errtime++;
		sleep(10);
		goto reconn;
	}
	memset(buf,0,4);
	char *p=strstr(recv,"+CSQ:");
	p=p+5;
	sscanf(p,"%[^,]",buf);
	//printf("buf=%s\n",buf);
        int signal=atoi(buf);
	printf("signal =%d\n",signal);
#if 1
	if(signal<9||signal==99){

		printf("signal fail\n");
		++errtime;
		sleep(10);
		goto reconn;
	}
#endif	
    
	at_write_serial(fd,atcmd,51);
	memset(recv,0,RECV_SIZE);
	at_read_serial(fd,recv,&timeout);
	printf("%s\n",recv);
	if(strstr(recv,"OK")==NULL){   
	    printf("AT^NETCFG fail\n");
	    ++errtime;
	    goto reconn;
         }
	
	char *atcmd2="AT$QCRMCALL=1,1,1,2,1\r\n";
	char *atcmd3="AT$QCRMCALL=1,1,1,1,1\r\n";

	at_write_serial(fd,"AT+COPS?\r\n",10);
	memset(recv,0,RECV_SIZE);
	at_read_serial(fd,recv,&timeout);
	printf("%s\n",recv);
	char buf2[10];
	char *qcrmcall;
	if(strstr(recv,"CHN-CT")!=NULL){
		memset(buf,0,4);
		memset(buf2,0,10);
		char *pCOPS=strstr(recv,"CHN-CT");
		sscanf(pCOPS,"%[^,],%s",buf2,buf);
		int form = atoi(buf);
		if(form==0||form==2){
		   	qcrmcall=atcmd3;		
		}
		else{
			qcrmcall=atcmd2;	
		}	
	}
	else if (strstr(recv, "CHINA TELECOM") != NULL)
	{
		memset(buf,0,4);
		memset(buf2,0,10);
		char *pCOPS=strstr(recv,"CHINA TELECOM");
		sscanf(pCOPS,"%[^,],%s",buf2,buf);
		int form = atoi(buf);
		if(form==0||form==2 ||form == 8){
		   	qcrmcall=atcmd3;		
		}
		else{
			qcrmcall=atcmd2;	
		}		
	}
	else{
		qcrmcall=atcmd2;
			
	}
	printf("qcrmcall=%s\n",qcrmcall);

	at_write_serial(fd,qcrmcall,23);
	memset(recv,0,RECV_SIZE);
	at_read_serial(fd,recv,&timeout);
	printf("%s\n",recv);
	if(strstr(recv,"$QCRMCALL:") == NULL){
    	    printf("RECV=%s\n",recv);
    	    printf("AT$QCRMCALL fail\n");
    	    goto reconn;
         }
#if 0
	at_write_serial(fd,"AT$QCRMCALL?\r\n",14);
	sleep(2);
	memset(recv,0,RECV_SIZE);
	at_read_serial(fd,recv,&timeout);
	printf("%s\n",recv);
	if(strstr(recv,"$QCRMCALL:")==NULL){
    	    printf("RECV=%s\n",recv);
    	    printf("QCRMCALL check fail\n");
    	    goto reconn;
         }
	 else{
	    memset(buf,0,4);
	    memset(buf2,0,10);
	    char *pCH=strstr(recv,"$QCRMCALL:");
	    pCH=pCH+10;
	    sscanf(pCH,"%[^,],%s",buf2,buf);
	    int con=atoi(buf2);
	    printf("connect=%d\n",con);	
	    printf("protocal=%s\n",buf);
	    if(con!=1){
	        printf("connect fail\n");
		goto reconn;
	    }
	}
#endif
	sleep(2);
	while (0)
        {
		printf("++++ snt check network ++++\n");
      //	printf("in while\n");
        	net = snt_ping();
         	if(net<0){
	        	printf("connect fail,try to reconnect..........\n");
			char *atcmd4="AT$QCRMCALL=0,1\r\n";
                        at_write_serial(fd,atcmd4,(int)strlen(atcmd4));
            
                        memset(recv,0,RECV_SIZE);
			at_read_serial(fd,recv,&timeout);
			printf("%s\n",recv);
			at_close_serial(fd);
			//system("rmmod gobinet");
			sleep(10);
			++errtime;
			
	        	goto reconn;
		} 
        	printf("network normal %d\n", net);			
		}
	
    return 0;
}
#endif
