#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

int main ()
{
    int fd;
    char my_message[40];
    unsigned long size;
    strcpy(my_message, "Shashi solutions:welcome to linux world");

    printf("Opening the device :[%d]\n",getpid());
    fd = open("/dev/mynull", O_RDWR);
    if(fd < 0)
    {
        printf("Device could not opened\n");
        return 1;
    }

    printf("Writing the message %s\n",my_message);
    size = (unsigned long)write(fd,my_message,strlen(my_message));
    printf("Bytes written %ld\n",size);
    close(fd);
    return 0;
}
