#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define RECV_SIZE 1024

static int at_set_speed(int fd,int speed)
{
    struct termios opt;
	int status;
	if(tcgetattr(fd,&opt)!=0)
		{
			printf("set_speed fail\n");
			return -1;
		}
	tcflush(fd,TCIOFLUSH);
	cfsetispeed(&opt,speed);
	cfsetospeed(&opt,speed);
	status = tcsetattr(fd,TCSANOW,&opt);
	if(status!=0)
		{
			printf("tcsetattr fail\n");
			return -1;
		}
	tcflush(fd,TCIOFLUSH);

	return 0;
	

}


static int at_open_serial(char *tty_name){
	int fd = open(tty_name,O_RDWR);
	if(fd<0){
		printf("open %s fail\n",tty_name);
		return -1;
	}	
	return fd;
	
}
static int at_setparity(int fd,int databits,int val,int stopbits ){
	struct termios operate;
	if(tcgetattr(fd,&operate)!=0){
		printf("setparity fail \n");
		return -1;
	}
	operate.c_cflag &=~CSIZE;
	switch(databits){

		case 5:
			operate.c_cflag |=CS5;
			break;
		case 6:
			operate.c_cflag |=CS6;
			break;
		case 7:		
			operate.c_cflag |=CS7;
			break;
		case 8:		
			operate.c_cflag |=CS8;
			break;
		default:
			printf("unsupport databits \n");
			return -1;	

	}
	switch(val)
	{
		case (-1):
			operate.c_cflag &=~PARENB;
			operate.c_iflag &=~INPCK;
			break;
		case 0:
			operate.c_cflag |= PARENB;
			operate.c_cflag &= ~PARODD;
			operate.c_iflag |= INPCK;
			break;
		case 1:
			operate.c_cflag |=PARENB;
			operate.c_cflag |=PARODD;
			operate.c_iflag |=INPCK;
			break;
		default:
			printf("unsupport valitdate bits\n");
			return -1;
	}
	switch(stopbits){
		case 1:
			operate.c_cflag &=~CSTOPB;
			break;
		case 2:
			operate.c_cflag |=CSTOPB;
			break;
		default:
			printf("unsupport stopbits\n");
			return -1;
	}
	operate.c_lflag &=~(ECHO | ICANON | ECHOE | ISIG);
	operate.c_oflag &=~OPOST;
	tcflush(fd,TCIOFLUSH);
	if(tcsetattr(fd,TCSANOW,&operate)!=0){
		printf("set serial fail \n");
		return -1;
	}
	return 0;	
}


static int at_set_serial(int fd,int speed,int databits,int val,int stopbits){
	at_set_speed(fd,speed);
	at_setparity(fd,databits,-1,stopbits);
	return 0;
	
}

int at_read_serial(int fd,char *buf, struct timeval *timeout){
		int ret;
		fd_set nfds;
		struct timeval tv;
		
		char *p_nbuf=buf;
		int i;
	#if 1
		for(i=0;i<RECV_SIZE - 1;i++){
			tv.tv_sec=timeout->tv_sec;
			tv.tv_usec=timeout->tv_usec;
			FD_ZERO(&nfds);
			FD_SET(fd,&nfds);
			ret=select(fd+1,&nfds,NULL,NULL,&tv);
			if(ret>0){
				ret=read(fd,p_nbuf,1);
				if(ret==1){
					p_nbuf++;
				}
				else{
					return -1;
				}
			}
			else if(ret==0){
				break;
			}
			else{
				return -1;	
				}
			
		}
		return p_nbuf-buf;
		
	#else
		
		ret = read(fd,buf,bufsize);
		return ret;
	#endif
}

int at_write_serial(int fd,char *buf,int size){
	if(write(fd,buf,size)<0)
		return -1;
	return 0;

}
static int at_close_serial(int fd){

	int ret=0;
	ret = close(fd);
	return ret;
}


