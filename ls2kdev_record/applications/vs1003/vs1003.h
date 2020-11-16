#ifndef VS1003_H
#define VS1003_H

#include <rtthread.h>
#define xDreq 38
#define xReset 37

#define xDcs 1
#define xCs 2

#define HIGH 1
#define LOW 0

#define _BV(n) 1<<n
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))


#define VS1003_ADPCM_BATCH_SIZE 256 
#define VS1003_CHUNK_SIZE 32
// VS1003 SCI Write Command byte is 0x02
#define VS_WRITE_COMMAND 0x02

// VS1003 SCI Read COmmand byte is 0x03
#define VS_READ_COMMAND 0x03

// SCI Registers

#define SCI_MODE  0x0
#define SCI_STATUS  0x1
#define SCI_BASS  0x2
#define SCI_CLOCKF  0x3
#define SCI_DECODE_TIME  0x4
#define SCI_AUDATA  0x5
#define SCI_WRAM  0x6
#define SCI_WRAMADDR  0x7
#define SCI_HDAT0  0x8
#define SCI_HDAT1  0x9
#define SCI_AIADDR  0xa
#define SCI_VOL  0xb
#define SCI_AICTRL0  0xc
#define SCI_AICTRL1  0xd
#define SCI_AICTRL2  0xe
#define SCI_AICTRL3  0xf
#define SCI_num_registers  0xf

// SCI_MODE bits

#define SM_DIFF  0
#define SM_LAYER12  1
#define SM_RESET  2
#define SM_OUTOFWAV  3
#define SM_EARSPEAKER_LO  4
#define SM_TESTS  5
#define SM_STREAM  6
#define SM_EARSPEAKER_HI  7
#define SM_DACT  8
#define SM_SDIORD  9
#define SM_SDISHARE  10
#define SM_SDINEW  11
#define SM_ADPCM  12
#define SM_ADCPM_HP  13
#define SM_LINE_IN  14

void vs1003_record_mode_init(void);
void vs1003_play_mode_init(void);
void vs1003_set_volume(uint8_t vol);
void vs1003_play_chunk(const uint8_t* , size_t );
uint16_t read_register(uint8_t);
void write_register(uint8_t, uint16_t );
void vs1003_reset();
rt_thread_t vs1003_start_record(void (*callback_p)(uint8_t * buffer,uint32_t buffer_size));
#endif