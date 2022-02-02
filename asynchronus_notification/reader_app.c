#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int fd;
char my_buff[4000];

void handler(int sig)
{
    printf("Got SIGIO\n");
    /*try to read 40 bytes from this device*/
    printf("Trying to read from the device\n");
    read(fd, my_buff, 20);
    printf("Read [%s]\n",my_buff);
}
int main()
{
    int oflags;
    char cmd[250];

    printf("[%d] - opening the device character\n",getpid());
    fd = open("/dev/mynull",O_RDONLY);

    perror("fd ");
    if(fd < 0)
    {
        printf("Device couldn't open..\n");
        return 1;
    }

    printf("Device opening with the ID [%d]\n",fd);

    fcntl(fd, F_SETOWN,getpid());//set this process as owner
    oflags = fcntl(fd, F_GETFL);//get the file flags now 
    fcntl(fd, F_SETFL, oflags | FASYNC);//enable async notice
    signal(SIGIO, handler);

    printf("I will now behave like a shell\n");
    while(1)
    {
        printf("Enter the command: ");
        gets(cmd);
        if(!strcmp("exit",cmd))
            break;
        system(cmd);
    }
    printf("Bye");
    close(fd);
    exit(0);
}
