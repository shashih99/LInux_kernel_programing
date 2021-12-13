#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int8_t write_buf[1024];
int8_t read_buf[1024];
int main()
{
	int choice;
	int fd;
	while(1)
	{	
		printf("*********************\n");
		printf("Enter the choice: 1.open file\n2.write\n3.read\n4.close\n5.exit\n");
		scanf("%d",&choice);
		switch(choice)
		{
			case 1:
				fd = open("/dev/shash_cdrv",O_RDWR);
				if(fd < 0)
				{
					printf("Not able to open the device...\n");
					exit(-1);
				}
				break;
			case 2:
				scanf("%[^\n]s",write_buf);
				printf("Data writen....\n");
				write(fd,write_buf,strlen(write_buf) + 1);
				printf("Done...\n");
				break;
			case 3:
				printf("Reading....");
				read(fd,read_buf,sizeof(read_buf));
				printf("Data = %s\n",read_buf);
				printf("Done....\n");
				break;
			case 4:
				close(fd);
				exit(0);
			default:
				printf("Wrong choice....\n");
				break;

		}
		close(fd);
	}
	return 0;
}
