#include <rtthread.h>
typedef struct
    {
        uint8_t reserved : 1 ;        ///< "Reserved" bit
        uint8_t noOperation : 2;      ///< "NOP" bits
        uint8_t pullUp : 1;           ///< "PULL_UP_EN" bit
        uint8_t sensorMode : 1;       ///< "TS_MODE" bit
        uint8_t rate : 3;             ///< "DR" bits
        uint8_t operatingMode : 1;    ///< "MODE" bit
        uint8_t pga : 3;              ///< "PGA" bits
        uint8_t mux : 3;              ///< "MUX" bits
        uint8_t singleStart : 1;      ///< "SS" bit
} Bits;  
union Config
{
    ///Structure of the config register of the ADS1118. (See datasheet [1])
    Bits bits;   
    uint16_t word;    ///< Representation in word (16-bits) format
    struct
    {
        uint8_t lsb;    ///< Byte LSB
        uint8_t msb;    ///< Byte MSB
    } byte;             ///< Representation in bytes (8-bits) format
};

//Input multiplexer configuration selection for bits "MUX"
//Differential inputs
#define DIFF_0_1   0b000    ///< Differential input: Vin=A0-A1
#define DIFF_0_3   0b001    ///< Differential input: Vin=A0-A3
#define DIFF_1_3   0b010    ///< Differential input: Vin=A1-A3
#define DIFF_2_3   0b011    ///< Differential input: Vin=A2-A3
                                //Single ended inputs
#define AIN_0   0b100       ///< Single ended input: Vin=A0
#define AIN_1   0b101       ///< Single ended input: Vin=A1
#define AIN_2   0b110       ///< Single ended input: Vin=A2
#define AIN_3   0b111       ///< Single ended input: Vin=A3


//Bit constants
//#define SCLK   2000000    ///< ADS1118 SCLK frequency: 4000000 Hz Maximum for ADS1118

// Used by "SS" bit
#define START_NOW   1    ///< Start of conversion in single-shot mode

// Used by "TS_MODE" bit
#define ADC_MODE   0     ///< External (inputs) voltage reading mode
#define TEMP_MODE   1    ///< Internal temperature sensor reading mode

// Used by "MODE" bit
#define CONTINUOUS   0     ///< Continuous conversion mode
#define SINGLE_SHOT   1    ///< Single-shot conversion and power down mode

// Used by "PULL_UP_EN" bit
#define DOUT_PULLUP   1       ///< Internal pull-up resistor enabled for DOUT ***DEFAULT
#define DOUT_NO_PULLUP   0    ///< Internal pull-up resistor disabled

// Used by "NOP" bits
#define VALID_CFG   0b01       ///< Data will be written to Config register
#define NO_VALID_CFG   0b00    ///< Data won't be written to Config register

// Used by "Reserved" bit
#define RESERVED   1    ///< Its value is always 1, reserved

/*Full scale range (FSR) selection by "PGA" bits. 
		 [Warning: this could increase the noise and the effective number of bits (ENOB). See tables above]*/
#define FSR_6144   0b000    ///< Range: ±6.144 v. LSB SIZE   187.5μV
#define FSR_4096   0b001    ///< Range: ±4.096 v. LSB SIZE   125μV
#define FSR_2048   0b010    ///< Range: ±2.048 v. LSB SIZE   62.5μV ***DEFAULT
#define FSR_1024   0b011    ///< Range: ±1.024 v. LSB SIZE   31.25μV
#define FSR_0512   0b100    ///< Range: ±0.512 v. LSB SIZE   15.625μV
#define FSR_0256   0b111    ///< Range: ±0.256 v. LSB SIZE   7.8125μV

/*Sampling rate selection by "DR" bits. 
		[Warning: this could increase the noise and the effective number of bits (ENOB). See tables above]*/
#define RATE_8SPS   0b000      ///< 8 samples/s, Tconv=125ms
#define RATE_16SPS   0b001     ///< 16 samples/s, Tconv=62.5ms
#define RATE_32SPS   0b010     ///< 32 samples/s, Tconv=31.25ms
#define RATE_64SPS   0b011     ///< 64 samples/s, Tconv=15.625ms
#define RATE_128SPS   0b100    ///< 128 samples/s, Tconv=7.8125ms
#define RATE_250SPS   0b101    ///< 250 samples/s, Tconv=4ms
#define RATE_475SPS   0b110    ///< 475 samples/s, Tconv=2.105ms
#define RATE_860SPS   0b111    ///< 860 samples/s, Tconv=1.163ms

int ads1118_test(int,char**);