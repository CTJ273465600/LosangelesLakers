

#include <rtthread.h>
#include "drv_spi.h"
#include "ads1118.h"

union Config configRegister;                                                                                     ///< Config register
Bits bits = {RESERVED, VALID_CFG, DOUT_PULLUP, ADC_MODE, RATE_8SPS, SINGLE_SHOT, FSR_0256, DIFF_0_1, START_NOW}; //Default values
uint8_t lastSensorMode = 3;                                                                                      ///< Last sensor mode selected (ADC_MODE or TEMP_MODE or none)
//uint8_t cs;                         ///< Chip select pin (choose one)
const float pgaFSR[8] = {6.144, 4.096, 2.048, 1.024, 0.512, 0.256, 0.256, 0.256};
const uint8_t CONV_TIME[8] = {125, 63, 32, 16, 8, 4, 3, 2}; ///< Array containing the conversions time in ms

void ads1118_begin()
{
    spi_set_csn(0xff); //disable csn1
    spi_init(0xb, 0x0, 0x1);
    configRegister.bits = bits;
}

/**
 * Setting the sampling rate specified in the config register
 * @param samplingRate It's the sampling rate: RATE_8SPS, RATE_16SPS, RATE_32SPS, RATE_64SPS, RATE_128SPS, RATE_250SPS, RATE_475SPS, RATE_860SPS
 */
void ads1118_setSamplingRate(uint8_t samplingRate)
{
    configRegister.bits.rate = samplingRate;
}

/**
 * Setting the full scale range in the config register
 * @param fsr The full scale range: FSR_6144 (±6.144V)*, FSR_4096(±4.096V)*, FSR_2048(±2.048V), FSR_1024(±1.024V), FSR_0512(±0.512V), FSR_0256(±0.256V). (*) No more than VDD + 0.3 V must be applied to this device.
 */
void ads1118_setFullScaleRange(uint8_t fsr)
{
    configRegister.bits.pga = fsr;
}

/**
 * Setting the inputs to be adquired in the config register. 
 * @param input The input selected: Diferential inputs: DIFF_0_1, DIFF_0_3, DIFF_1_3, DIFF_2_3. Single ended input: AIN_0, AIN_1, AIN_2, AIN_3
 */
void ads1118_setInputSelected(uint8_t input)
{
    configRegister.bits.mux = input;
}

/**
 * Setting to continuous adquisition mode
 */
void ads1118_setContinuousMode()
{
    configRegister.bits.operatingMode = CONTINUOUS;
}

/**
 * Setting to single shot adquisition and power down mode
 */
void ads1118_setSingleShotMode()
{
    configRegister.bits.operatingMode = SINGLE_SHOT;
}

/**
 * Disabling the internal pull-up resistor of the DOUT pin
 */
void ads1118_disablePullup()
{
    configRegister.bits.operatingMode = DOUT_NO_PULLUP;
}

/**
 * Enabling the internal pull-up resistor of the DOUT pin
 */
void ads1118_enablePullup()
{
    configRegister.bits.operatingMode = DOUT_PULLUP;
}

uint16_t ads1118_getADCValue(uint8_t inputs)
{
    uint16_t value;
    uint8_t dataMSB, dataLSB, configMSB, configLSB, count = 0;
    if (lastSensorMode == ADC_MODE) //Lucky you! We don't have to read twice the sensor
        count = 1;
    else
        configRegister.bits.sensorMode = ADC_MODE; //Sorry but we will have to read twice the sensor
    configRegister.bits.mux = inputs;
    do
    {

        spi_set_csn(0xbf); //enable csn1

        dataMSB = spi_write_for_response(configRegister.byte.msb);
        dataLSB = spi_write_for_response(configRegister.byte.lsb);
        configMSB = spi_write_for_response(configRegister.byte.msb);
        configLSB = spi_write_for_response(configRegister.byte.lsb);

        spi_set_csn(0xff); //disable csn1

        for (int i = 0; i < CONV_TIME[configRegister.bits.rate]; i++) //Lets wait the conversion time
            rt_thread_mdelay(1);
        count++;
    } while (count <= 1); //We make two readings because the second reading is the ADC conversion.
    value = (dataMSB << 8) | (dataLSB);
    return value;
}

/**
 * Getting the millivolts from the specified inputs
 * @param inputs Sets the inputs to be adquired. Diferential inputs: DIFF_0_1, DIFF_0_3, DIFF_1_3, DIFF_2_3. Single ended input: AIN_0, AIN_1, AIN_2, AIN_3
 * @return A double (32bits) containing the ADC value in millivolts
 */
double ads1118_getMilliVolts(uint8_t inputs)
{
    float volts;
    float fsr = pgaFSR[configRegister.bits.pga];
    uint16_t value;
    value = ads1118_getADCValue(inputs);
    if (value >= 0x8000)
    {
        value = ((~value) + 1); //Applying binary twos complement format
        volts = ((float)(value * fsr / 32768) * -1);
    }
    else
    {
        volts = (float)(value * fsr / 32768);
    }
    return volts * 1000;
}

int ads1118_test(int argc, char *argv[])
{
    ads1118_begin();

    /* Changing the sampling rate. 
       Available values: RATE_8SPS, RATE_16SPS, RATE_32SPS, RATE_64SPS, RATE_128SPS, RATE_250SPS, RATE_475SPS, RATE_860SPS */
    ads1118_setSamplingRate(RATE_860SPS); //Using the setter method to change the sampling rate
    //ads1118_configRegister.bits.rate=ads1118_RATE_8SPS;   //Driving the config register directly. Uncomment if you want to use this way

    /* Changing the input selected. 
       Available values: Diferential inputs: DIFF_0_1, DIFF_0_3, DIFF_1_3, DIFF_2_3. 
                         Single ended input: AIN_0, AIN_1, AIN_2, AIN_3*/
    ads1118_setInputSelected(AIN_0); //Using the setter method to change the input selected
    //ads1118_configRegister.bits.mux=ads1118_DIFF_0_1;     //Driving the config register directly. Uncomment if you want to use this way

    /* Changing the full scale range. 
       Available values: FSR_6144 (±6.144V)*, FSR_4096(±4.096V)*, FSR_2048(±2.048V), FSR_1024(±1.024V), FSR_0512(±0.512V), FSR_0256(±0.256V).
       (*) No more than VDD + 0.3 V must be applied to this device. */
    ads1118_setFullScaleRange(FSR_2048); //Using the setter method to change the full scale range
    //ads1118_configRegister.bits.pga=ads1118_FSR_0256;     //Driving the config register directly. Uncomment if you want to use this way

    /* Setting to continuous conversion mode */
    ads1118_setContinuousMode(); //Using the setter method to set it to continuous mode
    //ads1118_configRegister.bits.operatingMode=CONTINUOUS; //Driving the config register directly. Uncomment if you want to use this way

    /* Setting to single shot conversion mode */
    //ads1118_setSingleShotMode(); //Using the setter method to set it to "single shot conversion and power down" mode
    //ads1118_configRegister.bits.operatingMode=SINGLE_SHOT;//Driving the config register directly. Uncomment if you want to use this way

    /* Disabling the pull-up resistor */
    ads1118_disablePullup(); //Using the setter method to disable the pull-up resistor in Dout
    //ads1118_configRegister.bits.pullUp=PULLUP;            //Driving the config register directly. Uncomment if you want to use this way

    /* Enabling the pull-up resistor */
    //ads1118_enablePullup(); //Using the setter method to enable the pull-up resistor in Dout
    //ads1118_configRegister.bits.pullUp=NO_PULLUP;         //Driving the config register directly. Uncomment if you want to use this way
    char ch = 0;
    while (ch != 'q')
    {
        printf("%.4f\n", ads1118_getMilliVolts(AIN_0));
        scanf("%c", &ch);
    }
}
MSH_CMD_EXPORT(ads1118_test, ads1118_test);