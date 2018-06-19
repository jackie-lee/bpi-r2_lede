/* ping */
#include <stdio.h>
#include <stdlib.h>

int snt_ping(void)
{
	char cmd[80];
	int ret;
	FILE *p;
	int i=0;
	printf("ping baidu\n");
	ret=system("ping www.baidu.com -c 10 > ping.txt");
	if(ret !=0){
	    return -1;
	}
	p=fopen("ping.txt","r");
	if(!p)
		return -2;
	while(!feof(p)){
		//printf("start\n");
		fgets(cmd,80,p);
		if(strstr(cmd,"timeout")!=NULL||strstr(cmd,"unkonw host")!=NULL)
		{
			++i;
			printf("time out\n");
		}
	}

	if(i>9){
		printf("NET fail over 9 times,please check......\n");
		return -1;
	}
	
//	if(i<5)
//		system("rm ping.txt");
	return 0;
}
