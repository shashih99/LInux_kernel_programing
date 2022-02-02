#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <signal.h>

int main ()
{
    struct pollfd fds[1];
    int timeout = 10000;  //timeout poll
    int fd,rc,nbytes = 32;
    char *buffer;

    buffer = (char *)malloc(nbytes);

    printf("Opening the device :[%d]\n",getpid());
    fd = open("/dev/mynull", O_RDONLY);
    if(fd < 0)
    {
        printf("Not able to open the file..\n");
        exit(1);
    }
    printf("opening the file with file discriptor = %d\n",fd);

    fds[0].fd = fd;
    fds[0].events = POLLIN;

    while(1)
    {
        if((rc = poll (fds,1,timeout)) < 0)
        {
            perror("Failure in poll\n");
            exit(EXIT_FAILURE);
        }
        if(rc > 0)
        {
            printf("Poll returns %d revents = 0x%03x",rc,fds[0].revents);
            if(fds[0].revents & POLLIN)
            {
                rc = read(fd,buffer,nbytes);
                printf("reading %d bytes:%s\n",rc,buffer);
            }
            else
            {
                printf("POLLIN is not set\n");
            }
        }
        else
        {
            printf("poll timed out in %d milliseconds\n",timeout);
        }
    }
    close(fd);
    return 0;
}
 