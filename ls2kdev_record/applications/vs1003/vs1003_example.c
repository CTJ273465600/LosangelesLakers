#include <stdio.h>
#include <drv_spi.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/pin.h>
#include "vs1003.h"
#include "hello_mp3.h"
#include "morse1_mp3.h"
#include "adpcm-xq/adpcm-lib.h"
#include "c-goertzel/goertzel-dtmf.h"
#include "c-goertzel/goertzel.h"
#include "kiss_fft/kiss_fft.h"
#include <math.h>

#define NZEROS 4         //阶数
#define NPOLES 4
//#define GAIN   2.556465804e+02     //二阶
#define GAIN   6.520601554e+04       //四阶
//#define GAIN   1.084473754e+12     //十阶


//参数结构体
struct arg
{
    int argc;            //参数1
    char *argv[];        //参数2
}myarg;

struct adpcm_channel {
    int32_t pcmdata;                        // current PCM value
    int32_t error, weight, history [2];     // for noise shaping
    int8_t index;                           // current index into step size table
};

struct adpcm_context {
    struct adpcm_channel channels [2];
    int num_channels, lookahead, noise_shaping;
};

static unsigned char header[] = {
    0x52, 0x49, 0x46, 0x46, 0x1c, 0x10, 0x00, 0x00,
    0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
    0x14, 0x00, 0x00, 0x00, 0x11, 0x00, 0x01, 0x00,
    0x40, 0x1f, 0x00, 0x00, 0x75, 0x12, 0x00, 0x00,
    0x00, 0x01, 0x04, 0x00, 0x02, 0x00, 0xf9, 0x01,
    0x66, 0x61, 0x63, 0x74, 0x04, 0x00, 0x00, 0x00,
    0x5c, 0x1f, 0x00, 0x00, 0x64, 0x61, 0x74, 0x61,
    0xe8, 0x0f, 0x00, 0x00};
union bit_converter_t
{
    unsigned char b[4];
    uint32_t d;
} bit_converter;

void wrtie_header(FILE *f, uint32_t n, uint32_t fs)
{
    int F = n * VS1003_ADPCM_BATCH_SIZE + 52;
    int R = fs;
    int B = fs * VS1003_ADPCM_BATCH_SIZE / 505;
    int S = n * 505;
    int D = n * VS1003_ADPCM_BATCH_SIZE;

    bit_converter.d = F;
    for (int i = 0; i < 4; i++)
        header[4 + i] = bit_converter.b[i];

    bit_converter.d = R;
    for (int i = 0; i < 4; i++)
        header[24 + i] = bit_converter.b[i];

    bit_converter.d = B;
    for (int i = 0; i < 4; i++)
        header[28 + i] = bit_converter.b[i];

    bit_converter.d = S;
    for (int i = 0; i < 4; i++)
        header[48 + i] = bit_converter.b[i];

    bit_converter.d = D;
    for (int i = 0; i < 4; i++)
        header[56 + i] = bit_converter.b[i];

    for (int i = 0; i < sizeof(header); i++)
    {
        fputc(header[i], f);
    }
}

unsigned char db[VS1003_ADPCM_BATCH_SIZE * 2];

static float xv[NZEROS+1], yv[NPOLES+1];

struct goertzel_dtmf_state dtmf;

uint16_t convert(int16_t in) {
    return (uint16_t) 32768 + (uint16_t) in;
}

struct TUPLE_T{
    uint64_t length;
    uint8_t data_type;
    uint8_t *data;
};
typedef struct TUPLE_T* TUPLE;
static TUPLE new_tuple(uint64_t length, uint8_t data_type){
    TUPLE array=malloc(sizeof(struct TUPLE_T));
    array->data_type=data_type;
    array->data=malloc(length*data_type);
    array->length=length;
    return array;
}
static void del_tuple(TUPLE tuple){
    free(tuple->data);
    free(tuple);
}
double double_tuple_sum(uint16_t begin,uint16_t end,TUPLE tuple){
    double* src=tuple->data;
    double psum=0;
    for(int i=0;i<end-begin;i++){
        psum+=src[i];
    }
    return psum;
}
const uint16_t ndots=5;
int adpcm_decode(uint8_t *buffer,uint32_t buffersize){

    int16_t *pcm_buffer=(int16_t *)rt_malloc(buffersize*4);
    double samples_n=buffersize*2;
    adpcm_decode_block(pcm_buffer,buffer,buffersize,1);
    TUPLE sig=new_tuple(samples_n,sizeof(kiss_fft_cpx));
    TUPLE freq=new_tuple(samples_n,sizeof(kiss_fft_cpx));
    kiss_fft_cpx* src=sig->data;
    kiss_fft_cpx* dst=freq->data;
    for(int i=0;i<samples_n;i++){
        src[i].r=pcm_buffer[i];
        src[i].i=0;
    }
    kiss_fft_cfg cfg=kiss_fft_alloc(1024,0,0,0);
    kiss_fft(cfg,src,dst);
    TUPLE spec=new_tuple(1024,sizeof(double));
    double* spec_dst=spec->data;
    double psum=0.0;
    for(int i=0;i<1024;i++){
        kiss_fft_cpx* cpx=((kiss_fft_cpx*)(freq->data))+i;
        double p=sqrt((cpx->r*cpx->r)+(cpx->i*cpx->i));
        psum+=p;
        spec_dst[i]=p;
    }
    //rt_kprintf("s1:%ld\n",(int)log10(1+double_tuple_sum(250,262,spec)));
    return (int)log10(1+double_tuple_sum(282,294,spec));

    del_tuple(spec);
    del_tuple(freq);
    del_tuple(sig);
    rt_free(pcm_buffer);
}
int adpcm_goertzel_decode(uint8_t *buffer,uint32_t buffersize)
{
    struct goertzel_state s1;
    
    int16_t *pcm_buffer=(int16_t *)rt_malloc(buffersize*4);
    double samples_n=buffersize*2;
    adpcm_decode_block(pcm_buffer,buffer,buffersize,1);
    goertzel_init(&s1,8000,2250,1024);

    for (size_t s = 0; s < samples_n; s++)
    {
        goertzel_process_sample_pcm16(&s1,convert(pcm_buffer[s]));
    }
    //rt_kprintf("s1:%ld\t",(int)log10(1+goertzel_get_squared_magnitude(&s1)));

    return (int)log10(1+goertzel_get_squared_magnitude(&s1));

    goertzel_reset(&s1);
}
int adpcm_dtmf_decode(uint8_t *buffer,uint32_t buffersize)
{
    int16_t *pcm_buffer=(int16_t *)rt_malloc(buffersize*4);
    double samples_n=buffersize*2;
    adpcm_decode_block(pcm_buffer,buffer,buffersize,1);
    goertzel_dtmf_init(&dtmf, 8000, samples_n);
    float psum=0;
    for (size_t s = 0; s < samples_n; s++)
    {
        goertzel_dtmf_add_sample_pcm16(&dtmf, pcm_buffer[s]);
        psum+=(float)(pcm_buffer[s]+32768)/(float)(65535);
    }
    //rt_kprintf("p:%d\n",(long)psum);
    //if(psum>590.0)
    //rt_kprintf("code:%c\n",goertzel_dtmf_detect(&dtmf,3000000000, 3000000000));

    rt_free(pcm_buffer);

    return goertzel_dtmf_detect(&dtmf,3000000000, 3000000000);
}

//录音
void vs1003_record(int argc, char *argv[])     //argv[]<->输入的数据
{
    if (argc == 3)
    {
        uint16_t t, w;
        //amplifer_disable();
        vs1003_record_mode_init();
        rt_kprintf("fopen:%s\n", argv[1]);     //argv[1]<->文件名（wav）
        FILE *f = fopen(argv[1], "wb");        //fopen<->文件操作函数,w<->写入,b<->二进制
        int len = strtoul(argv[2], NULL, 0);   //strtoul<->字符串转换为数字,0<->十进制
        rt_kprintf("len:%d\n", len);
        wrtie_header(f, 2*len, 8000);

        
        while (len)
        {
            //rt_kprintf("batch:%d\n", __i);
            t = read_register(SCI_HDAT1);
            while (t >= VS1003_ADPCM_BATCH_SIZE)
            {
                if (t >= 896)
                {
                    rt_kprintf("Buffer Overflow!\n", t);
                    break;
                }

                for (int i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i = i + 2)
                {
                    w = read_register(SCI_HDAT0);
                    db[i] = w >> 8;
                    db[i + 1] = w & 0xFF;
                }

                for (int i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i++)
                {
                    //写入
                    fputc(db[i], f);
                }

                len--;
                if (len % 10 == 0)
                    rt_kprintf("len:%d\n", len);
                t -= VS1003_ADPCM_BATCH_SIZE;
            }
        }
        fclose(f);
        vs1003_reset();
        
    }
    else
    {
        printf("usage: vs1003_record filename length\n");
    }
}
MSH_CMD_EXPORT(vs1003_record, record);


//录音+戈泽尔滤波
void vs1003_record_filter_thread(struct arg *marg, int (*callback_p)(uint8_t * buffer,uint32_t buffer_size))
{
    if (marg->argc == 3)
    {
        uint16_t t, w;
        //amplifer_disable();
        vs1003_record_mode_init();
        rt_kprintf("fopen:%s\n", marg->argv[1]);     //argv[1]<->文件名（wav）
        FILE *f = fopen(marg->argv[1], "wb");        //fopen<->文件操作函数,w<->写入,b<->二进制
        FILE *g = fopen("wav_filter", "wb");        //fopen<->文件操作函数,w<->写入,b<->二进制
        int len = strtoul(marg->argv[2], NULL, 0);   //strtoul<->字符串转换为数字,0<->十进制
        rt_kprintf("len:%d\n", len);
        wrtie_header(f, 2*len, 8000);

        int16_t *pcm_buffer=(int16_t *)malloc(VS1003_ADPCM_BATCH_SIZE*8);
        size_t *outbufsize=(size_t *)malloc(VS1003_ADPCM_BATCH_SIZE*2);

        while (len)
        {
            //rt_kprintf("batch:%d\n", __i);
            t = read_register(SCI_HDAT1);
            while (t >= VS1003_ADPCM_BATCH_SIZE)
            {
                if (t >= 896)
                {
                    rt_kprintf("Buffer Overflow!\n", t);
                    break;
                }

                for (int i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i = i + 2)
                {
                    w = read_register(SCI_HDAT0);
                    db[i] = w >> 8;
                    db[i + 1] = w & 0xFF;
                    rt_kprintf("s1:%ld\t",db[i]);
                    rt_kprintf("s1:%ld\t",db[i+1]);
                }

                uint8_t *buffer=(uint8_t *)malloc(VS1003_ADPCM_BATCH_SIZE*2);
            
                for (int i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i++)                 //循环VS1003_ADPCM_BATCH_SIZE*2次
                {

                    //写入
                    fputc(db[i], f);

                    buffer[i] = (*callback_p)(&db, VS1003_ADPCM_BATCH_SIZE*2);
                    rt_kprintf("s1:%ld\t",buffer[i]);


                    //写入
                    fputc(buffer[i], g);
                }

                free(buffer);

                
                rt_kprintf("111");
                int32_t initial_deltas[2] = {1024, 1024};
                struct adpcm_context *p = adpcm_create_context (1, 0, 0, initial_deltas);
                rt_kprintf("222");
                adpcm_encode_block (&p, &buffer, &outbufsize, &pcm_buffer, VS1003_ADPCM_BATCH_SIZE*4);  //s1
                rt_kprintf("333");
                adpcm_free_context (&p);
                rt_kprintf("444");
                

                len--;
                if (len % 10 == 0)
                    rt_kprintf("len:%d\n", len);
                t -= VS1003_ADPCM_BATCH_SIZE;
            }
        }
        fclose(f);
        vs1003_reset();

    }
    else
    {
        printf("usage: vs1003_record_goertzel filename length\n");
    }
}

void vs1003_record_filter(int argc, char *argv[])
{
    if (argc == 3)
    {
        myarg.argc = argc;
        int i;
        for(i=0; i<myarg.argc; i++)
        {
            myarg.argv[i] = argv[i];
        }
        vs1003_record_filter_thread(&myarg, adpcm_goertzel_decode);
    }
    else
    {
        printf("usage: vs1003_record_filter filename length\n");
    }
    
}
MSH_CMD_EXPORT(vs1003_record_filter, record_filter);


//滤波
void filter_thread(struct arg *marg)
{
    if (marg->argc == 2)
    {

        //读文件
        FILE *f = fopen(marg->argv[1], "rb");           //fopen导入文件，argv[1]为文件名，"w+t"<->读取写入
        fseek(f, 0, SEEK_END);
        long flen = ftell(f);
        long buffersize = flen-60;
        long len = buffersize/(VS1003_ADPCM_BATCH_SIZE*2);
        long num = len;
        fseek(f, 0, SEEK_SET);
        rt_kprintf("flen:%d\n", flen);

        uint8_t *buffer = (uint8_t *)malloc(flen);
        //int x[flen];
        //int y[flen];
        fread(buffer, 1, flen, f);
        fclose(f);

        FILE *g = fopen("wav_filter", "wb");        //fopen<->文件操作函数,w<->写入,b<->二进制

        long i;

        struct goertzel_state s1;
        
        //rt_kprintf("1\n");
        while(len)
        {
            uint8_t *xbuffer = (uint8_t *)malloc(VS1003_ADPCM_BATCH_SIZE*2);
            int16_t *pcm_buffer=(int16_t *)malloc(VS1003_ADPCM_BATCH_SIZE*8);
            uint16_t *zbuffer= (uint16_t *)malloc(VS1003_ADPCM_BATCH_SIZE*8);     ///
            float pcm[VS1003_ADPCM_BATCH_SIZE*8];
            int z[VS1003_ADPCM_BATCH_SIZE*8];

            for (i = 0; i < VS1003_ADPCM_BATCH_SIZE * 8; i++)
            {
                pcm[i] = 0;
                z[i] = 0;
            }


            for (i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i++)
            {
                xbuffer[i] = buffer[(num-len)*VS1003_ADPCM_BATCH_SIZE*2+i+60];
                //rt_kprintf("s1:%ld\t",xbuffer[i]);
            }

            //ADPCM解码
            adpcm_decode_block(pcm_buffer,xbuffer,VS1003_ADPCM_BATCH_SIZE*2,1);

            //戈泽尔     进来512个数据，出去1个能量值！！！！
            goertzel_init(&s1,8000,3375,1024);

            for(i = 0; i < VS1003_ADPCM_BATCH_SIZE * 4; i++)
            {
                 goertzel_process_sample_pcm16(&s1,convert(pcm_buffer[i]));
                 zbuffer[i] = (int)log10(1+goertzel_get_squared_magnitude(&s1));
                 //rt_kprintf("s3:%ld\t",zbuffer[i]);
            }

            for (i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i++)
            {
                //写入
                fputc(zbuffer[i], g);
            }
           


            /*
            //巴特沃斯
            for (i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i++)
            {
                
                //rt_kprintf("s2: H:%X\tL:%X\n",((*(pcm_buffer+i))>>8)&0xff,(*(pcm_buffer+i))&0xff);
                pcm[i] = pcm_buffer[i];
                //pcm[i] = pcm[i]/65525;

                
                //二阶
                xv[0] = xv[1]; xv[1] = xv[2]; 
                xv[2] = pcm[i] / GAIN;
                yv[0] = yv[1]; yv[1] = yv[2]; 
                yv[2] =   (xv[2] - xv[0])
                         + ( -0.9921767002 * yv[0]) + ( -0.3886573907 * yv[1]);
                z[i] = yv[2];
                zbuffer[i] = z[i];

                //四阶
                xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; 
                xv[4] = pcm[i] / GAIN;
                yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; 
                yv[4] =   (xv[0] + xv[4]) - 2 * xv[2]
                        + ( -0.9889542499 * yv[0]) + ( -0.7738905308 * yv[1])
                        + ( -2.1402930332 * yv[2]) + ( -0.7782004019 * yv[3]);
                z[i] = yv[4];
                zbuffer[i] = z[i];

                //十阶
                xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; xv[6] = xv[7]; xv[7] = xv[8]; xv[8] = xv[9]; xv[9] = xv[10]; 
                xv[10] = pcm[i] / GAIN;
                yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; yv[6] = yv[7]; yv[7] = yv[8]; yv[8] = yv[9]; yv[9] = yv[10]; 
                yv[10] =   (xv[10] - xv[0]) + 5 * (xv[2] - xv[8]) + 10 * (xv[6] - xv[4])
                     + ( -0.9749041708 * yv[0]) + ( -1.9067923686 * yv[1])
                     + ( -6.3910817446 * yv[2]) + ( -8.2495150936 * yv[3])
                     + (-14.4607316410 * yv[4]) + (-12.7394843850 * yv[5])
                     + (-14.5344256230 * yv[6]) + ( -8.3338109256 * yv[7])
                     + ( -6.4892902795 * yv[8]) + ( -1.9459598092 * yv[9]);
                z[i] = yv[10];
                zbuffer[i] = z[i];
                
                //rt_kprintf("s3:%ld\n",zbuffer[i]);

                //写入
                fputc(zbuffer[i], g);

                //ybuffer[i] = adpcm_goertzel_decode(&xbuffer, VS1003_ADPCM_BATCH_SIZE*2);       //大于13000时出错：写入出界
            }
            */

            free(xbuffer);
            free(pcm_buffer);
            free(zbuffer);

            len--;
            //if (len % 10 == 0)
                //rt_kprintf("len:%d\n", len);

        }

        goertzel_reset(&s1); 
        
        
        free(buffer);
        fclose(g);

        rt_kprintf("filter accomplished.\n");
        
    }
    else
    {
        printf("usage: filter filename\n");
    }
        
}


//创建动态线程
void filter(int argc, char *argv[]){
    myarg.argc = argc;
    int i;
    for(i=0; i<myarg.argc; i++)
    {
        myarg.argv[i] = argv[i];
    }
    int tid4 = rt_thread_create("filter",
                               filter_thread,
                               &myarg,
                               10240000,
                               20,5);
    rt_thread_startup(tid4);
    return tid4;
}
MSH_CMD_EXPORT(filter, wav_filter);

/*
int vs1003_play_mode_test(int argc, char *argv[])
{
    vs1003_play_mode_init();
    vs1003_set_volume(0x00);
    while (1)
    {
        vs1003_play_chunk(Hello_MP3, sizeof(Hello_MP3));
        rt_thread_mdelay(500);
        rt_kprintf("hello.mp3\n");
    };
}
MSH_CMD_EXPORT(vs1003_play_mode_test, play);
*/

int vs1003_play_once(int argc, char *argv[])
{
    //amplifer_enable();
    vs1003_play_mode_init();
    vs1003_set_volume(0x00);

    //vs1003_play_chunk(Hello_MP3, sizeof(Hello_MP3));
    vs1003_play_chunk(Morse1_MP3, sizeof(Morse1_MP3));
    rt_thread_mdelay(500);
    rt_kprintf("morse1.mp3\n");
    //amplifer_disable();
}
MSH_CMD_EXPORT(vs1003_play_once, play);

int vs1003_play_wav(int argc, char *argv[])
{
    if (argc == 2)
    {
        //amplifer_enable();
        FILE *f = fopen(argv[1], "rb");           //fopen导入文件，argv[1]为文件名，"rb"为模式
        fseek(f, 0, SEEK_END);
        long flen = ftell(f);
        fseek(f, 0, SEEK_SET);
        rt_kprintf("flen:%d\n", flen);

        unsigned char *fbuffer = (unsigned char *)malloc(flen);
        fread(fbuffer, 1, flen, f);

        vs1003_play_mode_init();
        vs1003_set_volume(0x00);

        vs1003_play_chunk(fbuffer, flen);

        //rt_thread_mdelay(500);
        rt_kprintf(argv[1]);
        rt_kprintf("\n");
        free(fbuffer);
        fclose(f);
        //amplifer_disable();
    }
    else
    {
        rt_kprintf("usage: vs1003_play_wav filename(mp3,wav,wma,...,etc).\n");
    }
}
MSH_CMD_EXPORT(vs1003_play_wav, play);

