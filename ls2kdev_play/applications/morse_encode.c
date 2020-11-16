/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-05     bigmagic     first version
 * 2020-10-12     anyang       using for loongxin2k1000
 */

#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/pin.h>
#include "sound.h"
#include "morse_code.h"


rt_uint16_t morse_code[200];

//翻译函数声明
void translate(char a[31]);

//播放
void morse_play_once(int argc, char *argv[])
{   

if(argc == 3)
{
    int freq = strtoul(argv[1], NULL, 0);

    if(freq < 3500)
    {
         
        int speed = strtoul(argv[2], NULL, 0); 

        sound_init();   //启动发声模块

        //数据输入
        int size = 31;
        int i;
        char *str = (char *)malloc(size);     //申请内存块

        for(i=0; i<size; i++)
        {
            str[i] = '~';                   //str数组初始化
        }

        printf("请输入字符串(长度限制为30,以Ctrl+Enter结尾)：\n");

        fgets(str, size, stdin);  
        printf("读到的字符串长度为：%d\n", strlen(str));
        printf("读到的字符串为：%s",str);

        if(strlen(str) == 30)
        {
            rt_kprintf("\n");
        }

        translate(str);   //摩尔斯译码

        //发声
        i=0;
        rt_kprintf("播放中...");
        while(morse_code[i]!=NULL)
        {
            if(morse_code[i] == 3000)
            {
                sound_off();
                rt_thread_mdelay(600/speed);
                i++;
            }
            else if(morse_code[i] == 7000)
            {
                sound_off();
                rt_thread_mdelay(400/speed);
                i++;
            }
            else if(morse_code[i] == 100 || morse_code[i] == 300)
            {
                sound_set(freq, 50);

                sound_on();
                rt_thread_mdelay(morse_code[i]*2/speed);

                sound_off();
                rt_thread_mdelay(200/speed);

                i++;
            }
            else
            {
                i++;
            }
        }
        rt_kprintf("\n");
        return 0;
    }
    else
    {
        printf("usage: freq < 3500Hz\n");
    }
}
else
{
     printf("usage: morse_play_once freq speed\n");
}
    
}
MSH_CMD_EXPORT(morse_play_once, play)









//播放测试
void morse_play_test(int argc, char *argv[])
{   
    sound_init();

    //数据输入
    int code_size = 24;
    int i;
    char *code = (char *)malloc(code_size);     //申请内存块

    for(i=0; i<code_size; i++)
    {
        code[i] = '~';                     //code数组初始化
    }

    code = "WO AI LUO SHAN JI HU REN";

    printf("播放的示例字符串长度为：%d\n", 24);
    rt_kprintf("播放的示例字符串为：%s\n", code);

    translate(code);    //摩尔斯译码

    //发声
    i=0;
    rt_kprintf("播放中...");
    while(morse_code[i]!=NULL)
    {
        if(morse_code[i] == 3000)
        {
            sound_off();
            rt_thread_mdelay(300);
            i++;
        }
        else if(morse_code[i] == 7000)
        {
            sound_off();
            rt_thread_mdelay(200);
            i++;
        }
        else if(morse_code[i] == 100 || morse_code[i] == 300)
        {
            sound_set(3000, 50);

            sound_on();
            rt_thread_mdelay(morse_code[i]);

            sound_off();
            rt_thread_mdelay(100);

            i++;
        }
        else
        {
            i++;
        }
    }
    rt_kprintf("\n");
    return 0;
}
MSH_CMD_EXPORT(morse_play_test, play)

/*
//记录
void morse_record(int argc, char *argv[])
{   
    if (argc == 3)
    {
        rt_uint16_t t, w;

        //写入
        FILE *f = fopen(argv[1], "wt");            //fopen<->文件操作函数,argv[1]<->文件名,w+<->写入+更新
        rt_kprintf("fopen:%s\n", argv[2]);         //argv[2]<->输入文本
        fputs(argv[2], f);                         //向文件中写入字符串
        fclose(f);
        
        //读取
        char str[51];
        char *string = str;  //一定要先给指针变量初始化
        FILE *fp = fopen(argv[1], "rt");           //fopen导入文件，argv[1]为文件名，"rb"为模式
        if(fp == NULL) {
            perror("打开文件时发生错误");
            return(-1);
        }
        if( fgets (str, 50, fp)!=NULL ) {
            //向标准输出 stdout 写入内容
            puts(str);
        }
        fclose(fp);

    }
    else
    {
        printf("usage: morse_record filename text\n");
    }
}
MSH_CMD_EXPORT(morse_record, record)
*/

//翻译
void translate(char a[31])
{
    rt_kprintf("对应的摩尔斯码为：");
    //清空发声数组
    int q;
    for(q=0; q < 200; q++)
    {
        morse_code[q]=0;
    }

    //给发声数组赋值
    int i,j,k;
    j = 0;
    for(i = 0; i < 31; i++)
    {
        switch(a[i])
        {
            case ' ':
                k = 0;
                rt_kprintf("  ");
                while(morse_kong[k]!=NULL)
                {
                    morse_code[j] = morse_kong[k];
                    k++;
                    j++;
                }
            break;
            case 'A':
                k = 0;
                rt_kprintf(".- ");
                while(morse_A[k]!=NULL)
                {
                    morse_code[j] = morse_A[k];
                    k++;
                    j++;
                }
            break;
            case 'B':
                k = 0;
                rt_kprintf("-... ");
                while(morse_B[k]!=NULL)
                {
                    morse_code[j] = morse_B[k];
                    k++;
                    j++;
                }
            break;
            case 'C':
                k = 0;
                rt_kprintf("-.-. ");
                while(morse_C[k]!=NULL)
                {
                    morse_code[j] = morse_C[k];
                    k++;
                    j++;
                }
            break;
            case 'D':
                k = 0;
                rt_kprintf("-.. ");
                while(morse_D[k]!=NULL)
                {

                    morse_code[j] = morse_D[k];
                    k++;
                    j++;
                }
            break;
            case 'E':
                k = 0;
                rt_kprintf(". ");
                while(morse_E[k]!=NULL)
                {
                    morse_code[j] = morse_E[k];
                    k++;
                    j++;
                }
            break;
            case 'F':
                k = 0;
                rt_kprintf("..-. ");
                while(morse_F[k]!=NULL)
                {
                    morse_code[j] = morse_F[k];
                    k++;
                    j++;
                }
            break;
            case 'G':
                k = 0;
                rt_kprintf("--. ");
                while(morse_G[k]!=NULL)
                {
                    morse_code[j] = morse_G[k];
                    k++;
                    j++;
                }
            break;
            case 'H':
                k = 0;
                rt_kprintf(".... ");
                while(morse_H[k]!=NULL)
                {
                    morse_code[j] = morse_H[k];
                    k++;
                    j++;
                }
            break;
            case 'I':
                k = 0;
                rt_kprintf(".. ");
                while(morse_I[k]!=NULL)
                {
                    morse_code[j] = morse_I[k];
                    k++;
                    j++;
                }
            break;
            case 'J':
                k = 0;
                rt_kprintf(".--- ");
                while(morse_J[k]!=NULL)
                {
                    morse_code[j] = morse_J[k];
                    k++;
                    j++;
                }
            break;
            case 'K':
                k = 0;
                rt_kprintf("-.- ");
                while(morse_K[k]!=NULL)
                {
                    morse_code[j] = morse_K[k];
                    k++;
                    j++;
                }
            break;
            case 'L':
                k = 0;
                rt_kprintf(".-.. ");
                while(morse_L[k]!=NULL)
                {
                    morse_code[j] = morse_L[k];
                    k++;
                    j++;
                }
            break;
            case 'M':
                k = 0;
                rt_kprintf("-- ");
                while(morse_M[k]!=NULL)
                {
                    morse_code[j] = morse_M[k];
                    k++;
                    j++;
                }
            break;
            case 'N':
                k = 0;
                rt_kprintf("-. ");
                while(morse_N[k]!=NULL)
                {
                    morse_code[j] = morse_N[k];
                    k++;
                    j++;
                }
            break;
            case 'O':
                k = 0;
                rt_kprintf("--- ");
                while(morse_O[k]!=NULL)
                {
                    morse_code[j] = morse_O[k];
                    k++;
                    j++;
                }
            break;
            case 'P':
                k = 0;
                rt_kprintf(".--. ");
                while(morse_P[k]!=NULL)
                {
                    morse_code[j] = morse_P[k];
                    k++;
                    j++;
                }
            break;
            case 'Q':
                k = 0;
                rt_kprintf("--.- ");
                while(morse_Q[k]!=NULL)
                {
                    morse_code[j] = morse_Q[k];
                    k++;
                    j++;
                }
            break;
            case 'R':
                k = 0;
                rt_kprintf(".-. ");
                while(morse_R[k]!=NULL)
                {
                    morse_code[j] = morse_R[k];
                    k++;
                    j++;
                }
            break;
            case 'S':
                k = 0;
                rt_kprintf("... ");
                while(morse_S[k]!=NULL)
                {
                    morse_code[j] = morse_S[k];
                    k++;
                    j++;
                }
            break;
            case 'T':
                k = 0;
                rt_kprintf("- ");
                while(morse_T[k]!=NULL)
                {
                    morse_code[j] = morse_T[k];
                    k++;
                    j++;
                }
            break;
            case 'U':
                k = 0;
                rt_kprintf("..- ");
                while(morse_U[k]!=NULL)
                {
                    morse_code[j] = morse_U[k];
                    k++;
                    j++;
                }
            break;
            case 'V':
                k = 0;
                rt_kprintf("...- ");
                while(morse_V[k]!=NULL)
                {
                    morse_code[j] = morse_V[k];
                    k++;
                    j++;
                }
            break;
            case 'W':
                k = 0;
                rt_kprintf(".-- ");
                while(morse_W[k]!=NULL)
                {
                    morse_code[j] = morse_W[k];
                    k++;
                    j++;
                }
            break;
            case 'X':
                k = 0;
                rt_kprintf("-..- ");
                while(morse_X[k]!=NULL)
                {
                    morse_code[j] = morse_X[k];
                    k++;
                    j++;
                }
            break;
            case 'Y':
                k = 0;
                rt_kprintf("-.-- ");
                while(morse_Y[k]!=NULL)
                {
                    morse_code[j] = morse_Y[k];
                    k++;
                    j++;
                }
            break;
            case 'Z':
                k = 0;
                rt_kprintf("--.. ");
                while(morse_Z[k]!=NULL)
                {
                    morse_code[j] = morse_Z[k];
                    k++;
                    j++;
                }
            break;
            case '0':
                k = 0;
                rt_kprintf("----- ");
                while(morse_0[k]!=NULL)
                {
                    morse_code[j] = morse_0[k];
                    k++;
                    j++;
                }
            break;
            case '1':
            {
                k = 0;
                rt_kprintf(".---- ");
                while(morse_1[k]!=NULL)
                {
                    morse_code[j] = morse_1[k];
                    k++;
                    j++;
                }
            break;
            }
            case '2':
            {
                k = 0;
                rt_kprintf("..--- ");
                while(morse_2[k]!=NULL)
                {
                    morse_code[j] = morse_2[k];
                    k++;
                    j++;
                }
            break;
            }
            case '3':
            {
                k = 0;
                rt_kprintf("...-- ");
                while(morse_3[k]!=NULL)
                {
                    morse_code[j] = morse_3[k];
                    k++;
                    j++;
                }
            break;
            }
            case '4':
            {
                k = 0;
                rt_kprintf("....- ");
                while(morse_4[k]!=NULL)
                {
                    morse_code[j] = morse_4[k];
                    k++;
                    j++;
                }
            break;
            }
            case '5':
            {
                k = 0;
                rt_kprintf("..... ");
                while(morse_5[k]!=NULL)
                {
                    morse_code[j] = morse_5[k];
                    k++;
                    j++;
                }
            break;
            }
            case '6':
            {
                k = 0;
                rt_kprintf("-.... ");
                while(morse_6[k]!=NULL)
                {
                    morse_code[j] = morse_6[k];
                    k++;
                    j++;
                }
            break;
            }
            case '7':
            {
                k = 0;
                rt_kprintf("--... ");
                while(morse_7[k]!=NULL)
                {
                    morse_code[j] = morse_7[k];
                    k++;
                    j++;
                }
            break;
            }
            case '8':
            {
                k = 0;
                rt_kprintf("---.. ");
                while(morse_8[k]!=NULL)
                {
                    morse_code[j] = morse_8[k];
                    k++;
                    j++;
                }
            break;
            }
            case '9':
            {
                k = 0;
                rt_kprintf("----. ");
                while(morse_9[k]!=NULL)
                {
                    morse_code[j] = morse_9[k];
                    k++;
                    j++;
                }
                break;
            }
            case '.':
            {
                k = 0;
                rt_kprintf(".-.-.- ");
                while(morse_dian[k]!=NULL)
                {
                    morse_code[j] = morse_dian[k];
                    k++;
                    j++;
                }
                break;
            }
            case ':':
            {
                k = 0;
                rt_kprintf("---... ");
                while(morse_mao[k]!=NULL)
                {
                    morse_code[j] = morse_mao[k];
                    k++;
                    j++;
                }
                break;
            }
            case ',':
            {
                k = 0;
                rt_kprintf("--..-- ");
                while(morse_dou[k]!=NULL)
                {
                    morse_code[j] = morse_dou[k];
                    k++;
                    j++;
                }
                break;
            }
            case ';':
            {
                k = 0;
                rt_kprintf("-.-.-. ");
                while(morse_fen[k]!=NULL)
                {
                    morse_code[j] = morse_fen[k];
                    k++;
                    j++;
                }
                break;
            }
            case '\?':
            {
                k = 0;
                rt_kprintf("..--.. ");
                while(morse_wen[k]!=NULL)
                {
                    morse_code[j] = morse_wen[k];
                    k++;
                    j++;
                }
                break;
            }
            case '=':
            {
                k = 0;
                rt_kprintf("-...- ");
                while(morse_deng[k]!=NULL)
                {
                    morse_code[j] = morse_deng[k];
                    k++;
                    j++;
                }
                break;
            }
            case '\'':
            {
                k = 0;
                rt_kprintf(".----. ");
                while(morse_danyin[k]!=NULL)
                {
                    morse_code[j] = morse_danyin[k];
                    k++;
                    j++;
                }
                break;
            }
            case '/':
            {
                k = 0;
                rt_kprintf("-..-. ");
                while(morse_xie[k]!=NULL)
                {
                    morse_code[j] = morse_xie[k];
                    k++;
                    j++;
                }
                break;
            }
            case '!':
            {
                k = 0;
                rt_kprintf("-.-.-- ");
                while(morse_jing[k]!=NULL)
                {
                    morse_code[j] = morse_jing[k];
                    k++;
                    j++;
                }
                break;
            }
            case '-':
            {
                k = 0;
                rt_kprintf("-....- ");
                while(morse_hua[k]!=NULL)
                {
                    morse_code[j] = morse_hua[k];
                    k++;
                    j++;
                }
                break;
            }
            case '_':
            {
                k = 0;
                rt_kprintf("..--.- ");
                while(morse_xiahua[k]!=NULL)
                {
                    morse_code[j] = morse_xiahua[k];
                    k++;
                    j++;
                }
                break;
            }
            case '\"':
            {
                k = 0;
                rt_kprintf(".-..-. ");
                while(morse_shuangyin[k]!=NULL)
                {
                    morse_code[j] = morse_shuangyin[k];
                    k++;
                    j++;
                }
                break;
            }
            case '(':
            {
                k = 0;
                rt_kprintf("-.--. ");
                while(morse_zuokuo[k]!=NULL)
                {
                    morse_code[j] = morse_zuokuo[k];
                    k++;
                    j++;
                }
                break;
            }
            case ')':
            {
                k = 0;
                rt_kprintf("-.--.- ");
                while(morse_youkuo[k]!=NULL)
                {
                    morse_code[j] = morse_youkuo[k];
                    k++;
                    j++;
                }
                break;
            }
            case '$':
            {
                k = 0;
                rt_kprintf("...-..- ");
                while(morse_meiyuan[k]!=NULL)
                {
                    morse_code[j] = morse_meiyuan[k];
                    k++;
                    j++;
                }
                break;
            }
            case '&':
            {
                k = 0;
                rt_kprintf("...- ");
                while(morse_and[k]!=NULL)
                {
                    morse_code[j] = morse_and[k];
                    k++;
                    j++;
                }
                break;
            }
            case '@':
            {
                k = 0;
                rt_kprintf(".--.-. ");
                while(morse_at[k]!=NULL)
                {
                    morse_code[j] = morse_at[k];
                    k++;
                    j++;
                }
                break;
            }
            default:
                break;
        }
    }
    rt_kprintf("\n");
}