#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

int main()
{
	int fd,i;
	ssize_t ret;
	char my_buf[4000];

	for(i = 0 ; i < 4000; i++)
	{
		my_buf[i] = '*';
	}

	printf("[%d] - opening device shashi_cdev\n",getpid);
	fd = open("/dev/shash_cdrv",O_RDWR);
	if(fd < 0)
	{
		perror("Failed to open the device...");
		return errno;
	}
	printf("PID [%d]\n",getpid());
	getchar();

	ret = write(fd,my_buf, 4000);

	close(fd);

	return 0;
}
