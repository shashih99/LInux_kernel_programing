#include "header.h"

#define WR_DATA _IOW('a','a',int32_t *)
#define RD_DATA _IOR('a','b',int32_t*)


int main()
{
	int fd;
	int32_t val,num=0;
	printf("IOCTL based charcater device driver : Opening Driver from user space..\n");
	fd = open("/dev/mynull",O_RDWR);

	if(fd < 0)
	{
		printf("Cannot open the device file..\n");
		return 1;
	}
	printf("Enter the data to send...\n");
	scanf("%d",&num);
	printf("Writing value to the driver ..\n");
	ioctl(fd,WR_DATA,(int32_t *)&num);

	printf("Reading the data from driver...\n");
	ioctl(fd,RD_DATA,(int32_t *)&val);
	printf("value is %d\n",val);
	printf("Closing the driver...\n");
	close(fd);
}
