#include <stdio.h>
#include <drv_spi.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/pin.h>
#include "morse_code.h"
#include "morse_wav_ab.h"
#include "morse_wav_woaiqianrushi.h"
#include "morse_wav_thereisnothing.h"

char code[31];
//rt_uint16_t morse_code[200] = {MORSE_START, MORSE_DIAN, MORSE_HUA, MORSE_ZIFU_KONG, MORSE_CI_KONG, MORSE_ZIFU_KONG, MORSE_HUA, MORSE_DIAN, MORSE_DIAN, MORSE_DIAN, MORSE_ZIFU_KONG, MORSE_CI_KONG, MORSE_ZIFU_KONG, MORSE_HUA, MORSE_DIAN, MORSE_HUA, MORSE_DIAN, MORSE_ZIFU_KONG, MORSE_FINSH};

//参数结构体
struct arg
{
    int argc;            //参数1
    char *argv[];        //参数2
}myarg;

//逆翻译函数声明
void inverse_translate(int a[200]);

//摩尔斯音频处理
void morse_process_once_thread(struct arg *marg)
{
    if (marg->argc == 2)
    {
        //读文件
        FILE *f = fopen(marg->argv[1], "rb");           //fopen导入文件，argv[1]为文件名，"rb"为模式
        if(f == NULL) 
        {
            perror("找不到文件!");
        }
        fseek(f, 0, SEEK_END);
        long flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        rt_kprintf("flen:%d\n", flen);
        unsigned char *fbuffer = (unsigned char *)malloc(flen);     //申请内存块
        fread(fbuffer, 1, flen, f);          //读数据
        fclose(f);

        printf("读到的数据字节数为：%u\n", flen);

        //处理
        //数据分帧
        int wlen=200, inc=100;          //flen数据个数，wlen帧长，inc帧移
        int fn = (flen-wlen)/inc+1;    //fn帧数
        int j_len = flen-(fn-1)*inc;
        int arr[fn][wlen];
        int En[fn];
        int i, j, k;
        for(i=0; i<fn; i++)	         //i控制矩阵的行
        {
           for(j=0; j<wlen; j++)	//j控制矩阵的列
           {
                arr[i][j] = 0;	        //对矩阵arr进行初始化
           }
            En[i] = 0;	            //对数组En进行初始化
        }

        for(i=0; i<fn; i++)
        {
           if(i<fn-1)	            //向前fn-1帧（矩阵arr的前fn-1行）存入数据
           {
                for(j=0,k=0; j<wlen,k<wlen; j++,k++)	//前fn-1行每行存入wlen个数据
                {
                    arr[i][j] = fbuffer[inc*i+k];
                    if(arr[i][j] <= 6 )
                    {
                        arr[i][j] = 0;
                    }

                }
           }
           else	//向第fn帧（矩阵arr的第fn行）存入数据
           {
               for(j=0,k=0; j<j_len,k<=j_len; j++,k++)	//最后一行存入N-(fn-1)*inc个数据
               {
                    arr[i][j] = fbuffer[inc*i+k];
                    if(arr[i][j] <=6)
                    {
                        arr[i][j] = 0;
                    }
               }   
           }
        }

        printf("数据分帧后的帧数为：%u\n", fn);

        /*
        //打印arr数组
        for(i=0; i<fn; i++)
        {
            for(j=0; j<wlen; j++)
            {
                printf("%u", arr[i][j]);             //打印字符串
                rt_kprintf("\n");
            }
        }
        */
        
    
        //求每一帧的短时能量
        for(i=0; i<fn; i++)
        {
            for(j=0; j<wlen; j++)
            {
                En[i] += arr[i][j]*arr[i][j];
            }
        }

        //打印En数组
        for(i=0; i<fn; i++)
        {
            printf("%u", En[i]);             //打印字符串
            rt_kprintf("\n");
        }
        
        //判决
        int sta = 0;   //sta状态判断, 0初始, 1高电平, 2低电平;
        int st = 0;    //st数据开始判断,0没有数据,1数据开始
        int m,n;
        int jud[200];      //判决
        int H,L;
        int T = 1000000;   // T能量阈值
        k = 0;   

        for(i=0; i<200; i++)
        {
            jud[i] = 0;           //数组初始化
        }

        for(i=0; i<fn; i++)
        {
            if(sta == 0)
            {
                if(En[i]>T)           
                {
                    m = i;
                    sta = 1;
                }
            }
            if(sta == 1)
            {
                if(En[i]<T)
                {
                    n = i;
                    H = n-m;
                    if(H>=10 && H<35)
                    {
                        if(st == 1)
                        {
                            jud[k] = MORSE_DIAN;
                            k++;
                        }
                        else
                        {
                            jud[k] = MORSE_START;
                            k++;
                            jud[k] = MORSE_DIAN;
                            k++;
                            st = 1;
                        } 
                    }
                    else if(H>=35 && H<50)
                    {
                        if(st == 1)
                        {
                            jud[k] = MORSE_HUA;
                            k++;
                        }
                        else
                        {
                            jud[k] = MORSE_START;
                            k++;
                            jud[k] = MORSE_HUA;
                            k++;
                            st = 1;
                        }
                    }
                    sta = 2;
                }
            }
            if(sta==2)
            {
                if(En[i]>T)
                {
                    m = i;
                    L = m-n;
                    if(L>6 && L<=15)            //字符空判决门限
                    {
                        jud[k] = MORSE_ZIFU_KONG;
                        k++;
                    }
                    else if(L>15)             //词空判决门限
                    {
                        jud[k] = MORSE_ZIFU_KONG;
                        k++;
                        jud[k] = MORSE_CI_KONG;
                        k++;
                        jud[k] = MORSE_ZIFU_KONG;
                        k++;
                    }
                    sta = 1;
                }
            }
        }
 
        /*
        //打印jud数组
        i=0;
        while(jud[i] != 0)
        {
            printf("%u", jud[i]);             //打印字符串
            rt_kprintf("\n");
            i++;
        }
        */
        
        free(fbuffer);

        //显示
        inverse_translate(jud);            //逆翻译

        //printf("%s", code);             //打印字符串   
    }
    else
    {
        rt_kprintf("usage: morse_process_once filename(mp3,wav,wma,...,etc)\n");
    }
}

//摩尔斯音频处理测试
void morse_process_test_thread(struct arg *marg)
{
    if (marg->argc == 1)
    {
        //long flen = sizeof(Morse_WAV_AB);
        long flen = sizeof(Morse_WAV_WOAIQIANRUSHI);
        //long flen = sizeof(Morse_WAV_THEREISNOTHING);

        printf("读到的数据字节数为：%u\n", flen);

        int wlen=100, inc=100;          //flen数据个数，wlen帧长，inc帧移
        int fn = (flen-wlen)/inc+2;    //fn帧数
        int j_len = flen-(fn-1)*inc;
        int arr[fn][wlen];
        int En[fn];
        int i, j, k;

        //初始化
        for(i=0; i<fn; i++)	         //i控制矩阵的行
        {
            for(j=0; j<wlen; j++)	//j控制矩阵的列
            {
                arr[i][j] = 0;	        //对矩阵arr进行初始化
            }
            En[i] = 0;	            //对数组En进行初始化
        }

        //数据分帧
        for(i=0; i<fn; i++)
        {
           if(i<fn-1)	            //向前fn-1帧（矩阵arr的前fn-1行）存入数据
           {
                for(j=0,k=0; j<wlen,k<wlen; j++,k++)	//前fn-1行每行存入wlen个数据
                {
                    //arr[i][j] = Morse_WAV_AB[inc*i+k+60];
                    arr[i][j] = Morse_WAV_WOAIQIANRUSHI[inc*i+k+60];
                    //arr[i][j] = Morse_WAV_THEREISNOTHING[inc*i+k+60];
                }
           }
           else	//向第fn帧（矩阵arr的第fn行）存入数据
           {
               for(j=0,k=0; j<j_len,k<=j_len; j++,k++)	//最后一行存入N-(fn-1)*inc个数据
               {
                    //arr[i][j] = Morse_WAV_AB[k+60];
                    arr[i][j] = Morse_WAV_WOAIQIANRUSHI[inc*i+k+60];
                    //arr[i][j] = Morse_WAV_THEREISNOTHING[inc*i+k+60];
               }   
           }
        }
        
        printf("数据分帧后的帧数为：%u\n", fn);

        /*
        //打印arr数组
        for(i=0; i<fn; i++)
        {
            for(j=0; j<wlen; j++)
            {
                printf("%u", arr[i][j]);             //打印字符串
                rt_kprintf("\n");
            }
        }
        */
    
        //求每一帧的短时能量
        for(i=0; i<fn; i++)
        {
            for(j=0; j<wlen; j++)
            {
                En[i] += arr[i][j]*arr[i][j];
            }
        }
        
        /*
        //打印En数组
        for(i=0; i<fn; i++)
        {
            printf("%u", En[i]);             //打印字符串
            rt_kprintf("\n");
        }
        */
        

        //判决
        int sta = 0;   //sta状态判断, 0初始, 1高电平, 2低电平;
        int st = 0;    //st数据开始判断,0没有数据,1数据开始
        int m,n;
        int jud[200];      //判决
        int H,L;
        int T = 1000000;   // T能量阈值
        k = 0;   

        for(i=0; i<200; i++)
        {
            jud[i] = 0;           //数组初始化
        }

        for(i=0; i<fn; i++)
        {
            if(sta == 0)
            {
                if(En[i]>T)           
                {
                    m = i;
                    sta = 1;
                }
            }
            if(sta == 1)
            {
                if(En[i]<T)
                {
                    n = i;
                    H = n-m;
                    if(H>=3 && H<10)
                    {
                        if(st == 1)
                        {
                            jud[k] = MORSE_DIAN;
                            k++;
                        }
                        else
                        {
                            jud[k] = MORSE_START;
                            k++;
                            jud[k] = MORSE_DIAN;
                            k++;
                            st = 1;
                        }  
                    }
                    else if(H>=10 && H<20)
                    {
                        if(st == 1)
                        {
                            jud[k] = MORSE_HUA;
                            k++;
                        }
                        else
                        {
                            jud[k] = MORSE_START;
                            k++;
                            jud[k] = MORSE_HUA;
                            k++;
                            st = 1;
                        }
                    }
                    sta = 2;
                }
            }
            if(sta==2)
            {
                if(En[i]>T)
                {
                    m = i;
                    L = m-n;
                    if(L>8 && L<=60)            //字符空判决门限
                    {
                        jud[k] = MORSE_ZIFU_KONG;
                        k++;
                    }
                    else if(L>60)             //词空判决门限
                    {
                        jud[k] = MORSE_ZIFU_KONG;
                        k++;
                        jud[k] = MORSE_CI_KONG;
                        k++;
                        jud[k] = MORSE_ZIFU_KONG;
                        k++;
                    }
                    sta = 1;
                }
            }
        }
        
        /*
        //打印jud数组
        i=0;
        while(jud[i] != 0)
        {
            printf("%u", jud[i]);             //打印字符串
            rt_kprintf("\n");
            i++;
        }
        */

        //显示
        inverse_translate(jud);            //逆翻译

        //printf("%s", code);             //打印字符串  
    }
    else
    {
        rt_kprintf("usage: morse_process_test\n");
    }
}

//创建动态线程1
void morse_process_once(int argc, char *argv[]){
    myarg.argc = argc;
    int i;
    for(i=0; i<myarg.argc; i++)
    {
        myarg.argv[i] = argv[i];
    }
    int tid1 = rt_thread_create("morse_process_once",
                               morse_process_once_thread,
                               &myarg,
                               10240000,
                               20,5);
    rt_thread_startup(tid1);
    return tid1;
}
MSH_CMD_EXPORT(morse_process_once, process);

//创建动态线程2
void morse_process_test(int argc, char *argv[]){
    myarg.argc = argc;
    int i;
    for(i=0; i<myarg.argc; i++)
    {
        myarg.argv[i] = argv[i];
    }
    int tid2 = rt_thread_create("morse_process_test",
                               morse_process_test_thread,
                               &myarg,
                               10240000,
                               20,5);
    rt_thread_startup(tid2);
    return tid2;
}
MSH_CMD_EXPORT(morse_process_test, process);

//逆翻译
void inverse_translate(int a[200])
{
    rt_kprintf("识别出的字符串为：");

    //清空字符数组
    int q;
    for(q=0; q < 31; q++)
    {
        code[q]='~';
    }

    //给字符数组赋值
    int i,j,k;
    j = 0;
    for(i = 0; i < 200; i++)
    {
        switch(a[i])
        {
            case MORSE_START:
            case MORSE_ZIFU_KONG:
            {
                k = 1;
                int c[7];
                while(a[i+k] != MORSE_ZIFU_KONG && a[i+k] != MORSE_FINSH)
                {
                    c[k-1] = a[i+k];
                    //printf("%u", c[k-1]);
                    k++;
                }
                //摩尔斯码为1位
                if((k-1) == 1)
                {
                    //空格
                    if(c[0] == MORSE_CI_KONG)
                    {
                        code[j] = ' ';
                        j++;
                        rt_kprintf(" ");
                    }
                    //E
                    else if(c[0] == MORSE_DIAN)
                    {
                        code[j] = 'E';
                        j++;
                        rt_kprintf("E");
                    }
                    //T
                    else if(c[0] == MORSE_HUA)
                    {
                        code[j] = 'T';
                        j++;
                        rt_kprintf("T");
                    }
                    //~
                    else
                    {
                        code[j] = '~';
                        j++;
                        rt_kprintf("~");
                    }
                }
                //摩尔斯码为2位
                else if((k-1) == 2)
                {
                    //A
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA)
                    {
                        code[j] = 'A';
                        j++;
                        rt_kprintf("A");
                    }
                    //I
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN)
                    {
                        code[j] = 'I';
                        j++;
                        rt_kprintf("I");
                    }
                    //M
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA)
                    {
                        code[j] = 'M';
                        j++;
                        rt_kprintf("M");
                    }
                    //N
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN)
                    {
                        code[j] = 'N';
                        j++;
                        rt_kprintf("N");
                    }
                    //~
                    else
                    {
                        code[j] = '~';
                        j++;
                        rt_kprintf("~");
                    }
                }
                //摩尔斯码为3位
                else if((k-1) == 3)
                {
                    //D
                    if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN)
                    {
                        code[j] = 'D';
                        j++;
                        rt_kprintf("D");
                    }
                    //G
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_DIAN)
                    {
                        code[j] = 'G';
                        j++;
                        rt_kprintf("G");
                    }
                    //K
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_HUA)
                    {
                        code[j] = 'K';
                        j++;
                        rt_kprintf("K");
                    }
                    //O
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_HUA)
                    {
                        code[j] = 'O';
                        j++;
                        rt_kprintf("O");
                    }
                    //R
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_DIAN)
                    {
                        code[j] = 'R';
                        j++;
                        rt_kprintf("R");
                    }
                    //S
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN)
                    {
                        code[j] = 'S';
                        j++;
                        rt_kprintf("S");
                    }
                    //U
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_HUA)
                    {
                        code[j] = 'U';
                        j++;
                        rt_kprintf("U");
                    }
                    //W
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_HUA)
                    {
                        code[j] = 'W';
                        j++;
                        rt_kprintf("W");
                    }
                    //~
                    else
                    {
                        code[j] = '~';
                        j++;
                        rt_kprintf("~");
                    }
                }
                //摩尔斯码为4位
                else if((k-1) == 4)
                {
                    //B
                    if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN)
                    {
                        code[j] = 'B';
                        j++;
                        rt_kprintf("B");
                    }
                    //C
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_DIAN)
                    {
                        code[j] = 'C';
                        j++;
                        rt_kprintf("C");
                    }
                    //F
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_DIAN)
                    {
                        code[j] = 'F';
                        j++;
                        rt_kprintf("F");
                    }
                    //H
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN)
                    {
                        code[j] = 'H';
                        j++;
                        rt_kprintf("H");
                    }
                    //J
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_HUA)
                    {
                        code[j] = 'J';
                        j++;
                        rt_kprintf("J");
                    }
                    //L
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN)
                    {
                        code[j] = 'L';
                        j++;
                        rt_kprintf("L");
                    }
                    //P
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_DIAN)
                    {
                        code[j] = 'P';
                        j++;
                        rt_kprintf("P");
                    }
                    //Q
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_DIAN && c[3] == MORSE_HUA)
                    {
                        code[j] = 'Q';
                        j++;
                        rt_kprintf("Q");
                    }
                    //V
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_HUA)
                    {
                        code[j] = 'V';
                        j++;
                        rt_kprintf("V");
                    }
                    //X
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_HUA)
                    {
                        code[j] = 'X';
                        j++;
                        rt_kprintf("X");
                    }
                    //Y
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_HUA)
                    {
                        code[j] = 'Y';
                        j++;
                        rt_kprintf("Y");
                    }
                    //Z
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN)
                    {
                        code[j] = 'Z';
                        j++;
                        rt_kprintf("Z");
                    }
                    //&
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN)
                    {
                        code[j] = '&';
                        j++;
                        rt_kprintf("&");
                    }
                    //~
                    else
                    {
                        code[j] = '~';
                        j++;
                        rt_kprintf("~");
                    }
                }
                //摩尔斯码为5位
                else if((k-1) == 5)
                {
                    //0
                    if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_HUA)
                    {
                        code[j] = '0';
                        j++;
                        rt_kprintf("0");
                    }
                    //1
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_HUA)
                    {
                        code[j] = '1';
                        j++;
                        rt_kprintf("1");
                    }
                    //2
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_HUA)
                    {
                        code[j] = '2';
                        j++;
                        rt_kprintf("2");
                    }
                    //3
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_HUA && c[4] == MORSE_HUA)
                    {
                        code[j] = '3';
                        j++;
                        rt_kprintf("3");
                    }
                    //4
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_HUA)
                    {
                        code[j] = '4';
                        j++;
                        rt_kprintf("4");
                    }
                    //5
                    else if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_DIAN)
                    {
                        code[j] = '5';
                        j++;
                        rt_kprintf("5");
                    }
                    //6
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_DIAN)
                    {
                        code[j] = '6';
                        j++;
                        rt_kprintf("6");
                    }
                    //7
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_DIAN)
                    {
                        code[j] = '7';
                        j++;
                        rt_kprintf("7");
                    }
                    //8
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_DIAN && c[4] == MORSE_DIAN)
                    {
                        code[j] = '8';
                        j++;
                        rt_kprintf("8");
                    }
                    //9
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_DIAN)
                    {
                        code[j] = '9';
                        j++;
                        rt_kprintf("9");
                    }
                    //=
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_HUA)
                    {
                        code[j] = '=';
                        j++;
                        rt_kprintf("=");
                    }
                    ///
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_HUA && c[4] == MORSE_DIAN)
                    {
                        code[j] = '/';
                        j++;
                        rt_kprintf("/");
                    }
                    //(
                    else if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] ==MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_DIAN)
                    {
                        code[j] = '(';
                        j++;
                        rt_kprintf("(");
                    }
                    //~
                    else
                    {
                        code[j] = '~';
                        j++;
                        rt_kprintf("~");
                    }
                }
                //摩尔斯码为6位
                else if((k-1) == 6)
                {
                    //.
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_DIAN && c[3] == MORSE_HUA && c[4] == MORSE_DIAN && c[5] == MORSE_HUA)
                    {
                        code[j] = '.';
                        j++;
                        rt_kprintf(".");
                    }
                    //:
                    if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_DIAN && c[4] == MORSE_DIAN && c[5] == MORSE_DIAN)
                    {
                        code[j] = ':';
                        j++;
                        rt_kprintf(":");
                    }
                    //,
                    if(c[0] == MORSE_HUA && c[1] == MORSE_HUA && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_HUA && c[5] == MORSE_HUA)
                    {
                        code[j] = ',';
                        j++;
                        rt_kprintf(",");
                    }
                    //;
                    if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_DIAN && c[4] == MORSE_HUA && c[5] == MORSE_DIAN)
                    {
                        code[j] = ';';
                        j++;
                        rt_kprintf(";");
                    }
                    //?
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_DIAN && c[5] == MORSE_DIAN)
                    {
                        code[j] = '\?';
                        j++;
                        rt_kprintf("\?");
                    }
                    //'
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_HUA && c[5] == MORSE_DIAN)
                    {
                        code[j] = '\'';
                        j++;
                        rt_kprintf("\'");
                    }
                    //!
                    if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_DIAN && c[4] == MORSE_HUA && c[5] == MORSE_HUA)
                    {
                        code[j] = '!';
                        j++;
                        rt_kprintf("!");
                    }
                    //-
                    if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_DIAN && c[5] == MORSE_HUA)
                    {
                        code[j] = '-';
                        j++;
                        rt_kprintf("-");
                    }
                    //_
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_DIAN && c[5] == MORSE_HUA)
                    {
                        code[j] = '_';
                        j++;
                        rt_kprintf("_");
                    }
                    //"
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_DIAN && c[3] == MORSE_DIAN && c[4] == MORSE_HUA && c[5] == MORSE_DIAN)
                    {
                        code[j] = '\"';
                        j++;
                        rt_kprintf("\"");
                    }
                    //)
                    if(c[0] == MORSE_HUA && c[1] == MORSE_DIAN && c[2] == MORSE_HUA && c[3] == MORSE_HUA && c[4] == MORSE_DIAN && c[5] == MORSE_HUA)
                    {
                        code[j] = ')';
                        j++;
                        rt_kprintf(")");
                    }
                    //.--.-.//@
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_HUA && c[2] == MORSE_HUA && c[3] == MORSE_DIAN && c[4] == MORSE_HUA && c[5] == MORSE_DIAN)
                    {
                        code[j] = '@';
                        j++;
                        rt_kprintf("@");
                    }
                    //~
                    else
                    {
                        code[j] = '~';
                        j++;
                        rt_kprintf("~");
                    }
                }
                //摩尔斯码为7位
                else if((k-1) == 7)
                {
                    //$
                    if(c[0] == MORSE_DIAN && c[1] == MORSE_DIAN && c[2] == MORSE_DIAN && c[3] == MORSE_HUA && c[4] == MORSE_DIAN && c[5] == MORSE_DIAN && c[6] == MORSE_HUA)
                    {
                        code[j] = '$';
                        j++;
                        rt_kprintf("$");
                    }
                    //~
                    else
                    {
                        code[j] = '~';
                        j++;
                        rt_kprintf("~");
                    }
                }
                else if((k-1) == 0)
                {
                    //
                }
                else
                {
                    code[j] = '~';
                    j++;
                    rt_kprintf("~");
                }
                i = i + k - 1;
                //rt_kprintf("\n");
                break;
            }
            case MORSE_FINSH:
            {
                i = 200;
                break;
            }
            default:
                break;
        }
    }
}
