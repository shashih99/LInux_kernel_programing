#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main()
{
    int fd,i,ret;
    char my_buff[4000];

    printf("[%d] - opening the device character\n",getpid());
    fd = open("/dev/mynull",O_RDONLY);

    perror("fd ");
    if(fd < 0)
    {
        printf("Device couldn't open..\n");
        return 1;
    }
    /*trying to read the 40 bytes from this device*/
    printf("Trying to read from the device\n");
    if((ret = read(fd,my_buff,40)) < 0)
    {
        perror("\nRead");
        exit(1);
    }
    printf("Read message: %s\n",my_buff);
}
