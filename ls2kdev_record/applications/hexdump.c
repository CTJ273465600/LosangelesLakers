#include <rtthread.h>
#include <stdio.h>


int hexdump(int argc,char* argv[])
{
    if(argc==2){
        FILE *f=fopen(argv[1],"rb");
        int ctr=0;
        while(!feof(f))
        {

            rt_kprintf("%02x ", fgetc(f));
            if(++ctr%16==0)rt_kprintf("\n");
        }

    }
}
MSH_CMD_EXPORT(hexdump,hexdump);