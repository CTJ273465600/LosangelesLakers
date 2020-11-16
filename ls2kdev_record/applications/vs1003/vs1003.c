#define RT_USING_VS1003
#ifdef RT_USING_VS1003

#include <stdio.h>
#include <drv_spi.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/pin.h>
#include <drivers/pin.h>
static int amplifer_enable()
{
  rt_pin_mode(21,0);
  rt_pin_write(21,0);

}
static int amplifer_disable()
{
   rt_pin_mode(21,0);
   rt_pin_write(21,1);
}
#include "vs1003.h"

static int gpio_read(int pin)
{
  //rt_kprintf("gpio_read %d\n",pin);
  return rt_pin_read(pin);
}

static void gpio_write(int pin, int val)
{
  //rt_kprintf("gpio_write %d,%d\n",pin,val);
  return rt_pin_write(pin, val);
}

static void control_mode_on(void)
{
  //xdcs -> high (disable)
  //xcs  -> low  (enable)
  //1   1    0   1   1 1 1 1
  //3   2    1   0
  //nc  xdcs xcs rom

  spi_set_cs(xCs, 0);
  spi_set_cs(xDcs, 1);
}
static void control_mode_off(void)
{
  //xdcs -> high (disable)
  //xcs  -> low  (enable)
  //1   1    1   1   1 1 1 1
  //3   2    1   0
  //nc  xdcs xcs rom

  spi_set_csn(0xff);
}

static void data_mode_on(void)
{

  //xdcs -> low (enable)
  //xcs  -> high  (disable)
  //1   0    1   1   1 1 1 1
  //3   2    1   0
  //nc  xdcs xcs rom

  spi_set_cs(xCs, 1);
  spi_set_cs(xDcs, 0);
}

static void data_mode_off(void)
{
  //xdcs -> low (enable)
  //xcs  -> high  (disable)
  //1   1    1   1   1 1 1 1
  //3   2    1   0
  //nc  xdcs xcs rom
  spi_set_csn(0xff);
}

static void await_data_request(void)
{
  while (!gpio_read(xDreq))
    ;
}

static void delay_us(int t)
{
  // to be implementated
  while (t--)
    ;
}

void vs1003_reset()
{
  rt_pin_write(xReset, PIN_LOW);
  rt_thread_mdelay(1);
  rt_pin_write(xReset, PIN_HIGH);
}

uint16_t read_register(uint8_t _reg)
{
  await_data_request();
  control_mode_on();
  spi_write_for_response(VS_READ_COMMAND);
  spi_write_for_response(_reg);
  unsigned char response1 = spi_write_for_response(0xFF);
  unsigned char response2 = spi_write_for_response(0xFF);
  control_mode_off();
  return ((unsigned int)response1 << 8) | (response2 & 0xFF);
}

void write_register(uint8_t _reg, uint16_t _value)
{
  control_mode_on();

  delay_us(1); // tXCSS

  spi_write_for_response(VS_WRITE_COMMAND); // Write operation
  spi_write_for_response(_reg);             // Which register
  spi_write_for_response(_value >> 8);      // Send hi byte
  //rt_kprintf("HI:%2X\n",_value >> 8);
  spi_write_for_response(_value & 0xff); // Send lo byte
  //rt_kprintf("LO:%2X\n",_value & 0xff);
  delay_us(1); // tXCSH

  await_data_request();
  control_mode_off();
}

void vs1003_set_volume(uint8_t vol)
{
  uint16_t value = vol;
  value <<= 8;
  value |= vol;

  write_register(SCI_VOL, value); // VOL
}

void sdi_send_buffer(const uint8_t *data, size_t len)
{
  data_mode_on();
  while (len)
  {
    await_data_request();
    //delayMicroseconds(3);

    size_t chunk_length = min(len, VS1003_CHUNK_SIZE);
    len -= chunk_length;
    while (chunk_length--)
      spi_write_for_response(*data++);
  }
  data_mode_off();
}

void vs1003_play_chunk(const uint8_t *data, size_t len)
{
  sdi_send_buffer(data, len);
}

struct sync_thread_parameter_t
{
  uint8_t *data;
  size_t len;
}sync_thread_parameter;

void vs1003_play_chunk_sync_thread(struct sync_thread_parameter_t *para){
  sdi_send_buffer(para->data, para->len);
  while(1);
}

void vs1003_play_chunk_sync(const uint8_t *data, size_t len,int delay_ms){

  sync_thread_parameter.data=data;
  sync_thread_parameter.len=len;
  rt_thread_t thread=RT_NULL;
  thread=rt_thread_create("sync_chunk",
                            vs1003_play_chunk_sync_thread, &sync_thread_parameter,
                            10240,
                            25, 5);

    /* 如果获得线程控制块，启动这个线程 */
    if (thread != RT_NULL)
    {
      rt_thread_startup(thread);
      rt_thread_mdelay(delay_ms);
      rt_thread_delete(thread);
    }
        
}

void vs1003_play_mode_init(void)
{

  // Keep the chip in reset until we are ready
  rt_pin_mode(xReset, PIN_MODE_OUTPUT);
  rt_pin_write(xReset, PIN_LOW);

  // The SCI and SDI will start deselected
  spi_set_cs(xCs, 1);
  spi_set_cs(xDcs, 1);

  // DREQ is an input
  rt_pin_mode(xDreq, PIN_MODE_INPUT);

  // Boot VS1003
  rt_kprintf("Booting VS1003...\r\n");
  rt_thread_mdelay(1);

  spi_init(0b1011, 0, 0);

  // release from reset
  rt_pin_write(xReset, PIN_HIGH);

  // Declick: Immediately switch analog off
  write_register(SCI_VOL, 0xffff); // VOL

  /* Declick: Slow sample rate for slow analog part startup */
  write_register(SCI_AUDATA, 10);

  rt_thread_mdelay(100);

  /* Switch on the analog parts */
  write_register(SCI_VOL, 0xfefe); // VOL

  rt_kprintf("VS1003 still booting\r\n");

  write_register(SCI_AUDATA, 44101); // 44.1kHz stereo

  write_register(SCI_VOL, 0x2020); // VOL

  // soft reset
  write_register(SCI_MODE, _BV(SM_SDINEW) | _BV(SM_RESET));
  rt_thread_mdelay(1);
  await_data_request();
  write_register(SCI_CLOCKF, 0xB800); // Experimenting with higher clock settings
  rt_thread_mdelay(1);
  await_data_request();

  // Now you can set high speed SPI clock
  //SPI.setClockDivider(SPI_CLOCK_DIV4); // Fastest available
  spi_init(0b1000, 0, 0);

  rt_kprintf("VS1003 OK\r\n");
}

void vs1003_record_mode_init()
{
  // @TODO
  rt_kprintf("vs1003_init\n");

  //db = (unsigned char *)rt_malloc(VS1003_ADPCM_BATCH_SIZE * 2);
  //rt_kprintf("allocating:%x\n", db);

  rt_pin_mode(xReset, PIN_MODE_OUTPUT);
  rt_pin_mode(xDreq, PIN_MODE_INPUT);
  rt_pin_write(xReset, PIN_LOW);
  //spi_init(0b1011, 0, 0);
  spi_init(0b1000, 0, 0);

  spi_set_csn(0xff);
  rt_pin_write(xReset, PIN_HIGH);

  
  write_register(SCI_BASS, 0);
  write_register(SCI_CLOCKF, 0x4430); // MUL=2 ADD=0 FREQ=1072 XTAL=FREQ*4000+8000000=12.288MHz
  rt_thread_mdelay(10);
  write_register(SCI_AICTRL0, 12); // div=12 @ 12.288MHz XTAL, fs=(MUL*XTAL)/(256*div)=8000Hz
  rt_thread_mdelay(10);

  write_register(SCI_AICTRL1, 1024);// DigitalGain=1 When SCI_AICTRL1=1024
  //write_register(SCI_AICTRL1, 0); // Auto Gain Control Mode Activated When SCI_AICTRL1=0
  rt_thread_mdelay(10);

  write_register(SCI_MODE, 0x3804); // SM_RESET=1 SM_ADPCM_=1 SM_ADPCM_HP(500~3000)=1 SM_LINE_IN=1  MSb_first(sdi_msb_first)
  rt_thread_mdelay(10);
  write_register(SCI_VOL, 0xfefe); // total slience 
}

void vs1003_start_record_thread(void (*callback_p)(uint8_t * buffer,uint32_t buffer_size)) // 0.128 s/CHUNK @ fs=8000
{

        uint8_t db[VS1003_ADPCM_BATCH_SIZE*2]; 
        // ATTENTION!! db is statically allocated, so the callback function may need to copy the data out of buffer in case of unexcepted data loss. 

        //int16_t pcm_db[VS1003_ADPCM_BATCH_SIZE*2];
        uint16_t t, w;
        
        amplifer_disable();
        vs1003_record_mode_init();

        while (1)
        {
            //rt_kprintf("batch:%d\n", __i);
            t = read_register(SCI_HDAT1);
            while (t >= VS1003_ADPCM_BATCH_SIZE /*&& t < 896*/)
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
                
                (*callback_p)(&db,VS1003_ADPCM_BATCH_SIZE*2);// ATTENTION! CALLBACK FUNCTION MAY BLOCK THE RECORDING PROCESS!!
                /*
                for (int i = 0; i < VS1003_ADPCM_BATCH_SIZE * 2; i++)
                {
                    fputc(db[i], f);
                }
                */
                
                t -= VS1003_ADPCM_BATCH_SIZE;
            }
        }
        vs1003_reset();
}

rt_thread_t vs1003_start_record(void (*callback_p)(uint8_t * buffer, uint32_t buffer_size))    //11.10
{
  int tid = rt_thread_create("recorder",
                            vs1003_start_record_thread, callback_p,
                            20480,
                            20,5);
  rt_thread_startup(tid);
  return tid;
}

#endif