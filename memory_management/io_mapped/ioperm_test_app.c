#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define   BASEPORT   0x70

int main()
{
    char val = 0;
    /*Get access to the port*/
    if(ioperm(BASEPORT,4,1))
    {
        perror("ioperm");
        exit(1);
    }

    val = inb(BASEPORT + 1);
    printf("Seconds = %x\n",val);

    /*we dont need anymore*/
    if(ioperm(BASEPORT,3,0))
    {

    }
    return 0;
}