#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#include <config.h>

int main(int argc,char *argv[])
{
	int fd;

	uint32_t p1,ps1;
	uint32_t p2,ps2;
	uint32_t p3,ps3;
	uint32_t p4,ps4;

	uint8_t pt1,pt2,pt3,pt4;
	uint8_t block[512];

	if(argc != 2)
	{
		printf("usage: %s filename.\n",argv[0]);
		exit(0);
	}

	p1=p2=p3=p4=0;
	ps1=ps2=ps3=ps4=0;
	pt1=pt2=pt3=pt4=0;

	memset(block,0,512);

	/*It was defined in config.h */
	p1 = MBR_P1_OFFSET;
	ps1 = MBR_P1_SIZE;
	pt1 = MBR_P1_TYPE;

	p2 = MBR_P2_OFFSET;
	ps2 = MBR_P2_SIZE;
	pt2 = MBR_P2_TYPE;

	p3 = MBR_P3_OFFSET;
	ps3 = MBR_P3_SIZE;
	pt3 = MBR_P3_TYPE;

	p4 = MBR_P4_OFFSET;
	ps4 = MBR_P4_SIZE;
	pt4 = MBR_P4_TYPE;

	block[0] = 0x80;
	block[1] = 0x00;
	block[2] = 0x00;
	block[3] = 0x10;
	block[4] = 0x00;
	block[5] = 0x00;
	block[6] = 0x00;
	block[7] = 0x00;

	block[0x1fe] = 0x55;
	block[0x1ff] = 0xaa;

	p1 /= 512;
	p2 /= 512;
	p3 /= 512;
	p4 /= 512;

	ps1 /= 512;
	ps2 /= 512;
	ps3 /= 512;
	//ps4 /= 512;

	memcpy(block+0x1c6,&p1,sizeof(uint32_t));
	memcpy(block+0x1d6,&p2,sizeof(uint32_t));
	memcpy(block+0x1e6,&p3,sizeof(uint32_t));
	memcpy(block+0x1f6,&p4,sizeof(uint32_t));

	memcpy(block+0x1ca,&ps1,sizeof(uint32_t));
	memcpy(block+0x1da,&ps2,sizeof(uint32_t));
	memcpy(block+0x1ea,&ps3,sizeof(uint32_t));
	memcpy(block+0x1fa,&ps4,sizeof(uint32_t));

	memcpy(block+0x1c2,&pt1,sizeof(uint8_t));
	memcpy(block+0x1d2,&pt2,sizeof(uint8_t));
	memcpy(block+0x1e2,&pt3,sizeof(uint8_t));
	memcpy(block+0x1f2,&pt4,sizeof(uint8_t));

	fd = open(argv[1],O_RDWR | O_TRUNC | O_CREAT,0777);
	if(fd < 0)
	{
		printf("open %s failed.\n",argv[1]);
		exit(1);
	}

	write(fd,block,512);

	close(fd);
	return 0;
}
