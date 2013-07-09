#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <limits.h>  		/*用于检测整型数据数据类型的表达值范围。*/
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>					/* ioctl 命令*/
#include <linux/if_ether.h>             /* ethhdr 结构*/
#include <net/if.h>                     /* ifreq 结构*/
#include <netinet/in.h>                 /* in_addr 结构*/
#include <linux/ip.h>                   /* iphdr 结构*/
#include <linux/udp.h>                  /* udphdr 结构*/
#include <linux/tcp.h>                  /* tcphdr 结构*/

#define PACKET_TEMP_SIZE 99999

char ef[PACKET_TEMP_SIZE][ETH_FRAME_LEN];
int packet_head4s = 0;
int packet_head4l = 0;
int packet_tail = 0;
unsigned int size;
unsigned int packet_id = 0;
int fd;				/*fd式套接口的描述符*/

void *statistic()
{
	printf("流量统计后台程序开始\n");
	while(1)
	{
		if(packet_head4s != packet_tail)
		{
			printf("show statistic table\npacketID: %u\n",packet_head4s);
			if(packet_head4s < PACKET_TEMP_SIZE)
			{
				++packet_head4s;
			}
			else packet_head4s = 0;
		}
	}
	printf("流量统计后台程序结束\n");
	pthread_exit(NULL);
}

void *log()
{	
	printf("日志后台程序开始\n");
	while(1)
	{
		if(packet_head4l != packet_tail)
		{
			printf("show log\npacketID: %u\n",packet_head4l);
			if(packet_head4l < PACKET_TEMP_SIZE)
			{
				++packet_head4l;
			}
			else packet_head4l = 0;
		}
	}
	printf("日志后台程序结束\n");
	pthread_exit(NULL);
}

void *test()
{
	
}

void *catch()
{
	while(1)
	{
		size = read(fd, ef[packet_tail], ETH_FRAME_LEN);
		if(packet_tail < PACKET_TEMP_SIZE)
		{
			++packet_tail;
		}
		else packet_tail = 0;
	}
	pthread_exit(NULL);
}

int main()
{
	char command[14];
	pthread_t thread_stat;
	pthread_t thread_log;
	pthread_t thread_catch;
	
	memset(ef, 0, sizeof(ef));
	
	fd = socket(AF_INET, SOCK_PACKET, htons(0x0003));
	if(fd < 0)
	{
		printf("套接字创建失败\n");
		return -1;
	}

	char *ethname = "eth0";		/*对网卡eht0进行混杂设置*/
	struct ifreq ifr;			/*网络接口结构*/
	int err;

	strcpy(ifr.ifr_name, ethname);			/*将“eth0”写入ift结构的一个字段中*/
	err = ioctl(fd, SIOCGIFFLAGS, &ifr);	/*获得eth0的标志位值*/
	if(err < 0)				/*判断是否取出出错*/
	{
		close(fd);
		printf("不能取出标志位值\n");
		return -1;
	}
	ifr.ifr_flags |= IFF_PROMISC;			/*保留原来的设置的情况下，在标志位中加入“混杂”方式*/
	err = ioctl(fd, SIOCSIFFLAGS, &ifr);	/*将标志位设置写入*/
	if(err < 0)				/*判断是否写入出错*/
	{
		printf("标志位写入失败\n");
		return -2;
	}
	pthread_create(&thread_catch,NULL,catch,NULL);		/*创建包捕获线程*/
	pthread_create(&thread_stat,NULL,statistic,NULL);	/*创建状态检测线程*/
	pthread_create(&thread_log,NULL,log,NULL);			/*创建日志记录线程*/
		
	while(gets(command))
	{
		if(strcmp(command,"show") == 0)
		{
			printf("Hei, I'm function show\n");
			continue;
		}
		else if(strcmp(command,"add") == 0)
		{
			printf("Hei, I'm function add\n");
			continue;
		}
		else if(strcmp(command,"quit") == 0)
		{
			break;
		}
	}
	return 0;
}
