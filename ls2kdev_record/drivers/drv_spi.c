#include <stdio.h>
//#include "include/fcr.h"
#include <stdlib.h>
#include <ctype.h>

#include <rtthread.h>
#include <drivers/spi.h>

#include "drv_spi.h"
//#include <include/types.h>

#ifdef RT_USING_SPI



//0100:div8
//1011:div4096
void spi_init(uint8_t spre_spr,uint8_t copl,uint8_t cpha) 
{ 
	rt_kprintf("SPI initiating with spre_spr:%2X ,copl:%2X ,cpha:%2X\n",spre_spr,copl,cpha);
	int d;
  	SET_SPI(SPSR, 0xc0|(spre_spr&0b00000011)); 
  	SET_SPI(PARAM, 0x40);             
  	SET_SPI(PARAM2,0x01); 
 	SET_SPI(SPER, (spre_spr&0b00001100)>>2); 
  	SET_SPI(SPCR, 0x50|copl<<3|cpha<<2); //0 极性 1 相位
	SET_SPI(SOFTCS,0xff);
}


void spi_set_csn(uint8_t val) //old method
{
	SET_SPI(SOFTCS,val);
}
#define RT_USING_SPI_GPIOCS

#ifdef RT_USING_SPI_GPIOCS
#include <drivers/pin.h>
#endif
void spi_set_cs(unsigned char cs, int new_status)
{

    if(cs<4){
        unsigned char val = 0;

    val = GET_SPI(SOFTCS);
    val |= 0x01 << cs ; //对应的csen=1  
    if (new_status)         // cs = 1
    {
        val |= (0x10 << cs);            // 指定csn=1
    }
    else                    // cs = 0
    {
        val &= ~(0x10 << cs);           // 指定csn=0
    }
    SET_SPI(SOFTCS,val);

    return ;
    }
    #ifdef RT_USING_SPI_GPIOCS
    else{
        rt_kprintf("[Warnning] GPIOCS is an experimental feature: \n ");
        rt_kprintf("[Warnning] GPIO%d will be set to OUTPUT with value %d \n ",cs,new_status);
        rt_pin_mode(cs,PIN_MODE_OUTPUT);
        rt_pin_write(cs,new_status);
    }
    #endif
}

uint8_t spi_write_for_response(uint8_t data)
{
	uint8_t val;
	SET_SPI(TXFIFO,data);
	while((GET_SPI(SPSR))&RFEMPTY);//wait for echo
	val = GET_SPI(RXFIFO);
	return val;
}



int cmd_spi_init(int argc,char* argv[])
{
	uint8_t spre_spr,cpol,cpha;
	switch (argc)
	{
	case 2:
		
		spre_spr=strtoul(argv[1], NULL, 0);
		spi_init(spre_spr,0,0);
		break;
	case 4:
		
		spre_spr=strtoul(argv[1], NULL, 0);
		cpol=strtoul(argv[2], NULL, 0);
		cpha=strtoul(argv[3], NULL, 0);
		spi_init(spre_spr,0,0);
		break;
	
	default:
		printf("\nusage : cmd_spi_init spre_spr <cpol> <cpha>\n(cmd_spi_init 0x4 0x0 0x0)\n0x4:div8 0xb:div4096\n");
		break;
	}
}
MSH_CMD_EXPORT(cmd_spi_init,cmd_spi_init);

int cmd_spi_set_csn(int argc,char* argv[])
{
	uint8_t val,csn;
	switch (argc)
	{
	case 3:
		csn=strtoul(argv[1], NULL, 0);
		val=strtoul(argv[2], NULL, 0);
		spi_set_cs(csn,val);
		break;
	
	default:
		printf("usage:cmd_spi_set_csn csn val\n(0xbf for csn1 enable,0xff for csn1 disable)\n");
		break;
	}
}
MSH_CMD_EXPORT(cmd_spi_set_csn,cmd_spi_set_csn);

int cmd_spi_write(int argc,char* argv[])
{
	uint8_t data,resp;
	switch (argc)
	{
	case 2:
		
		data=strtoul(argv[1], NULL, 0);
		resp=spi_write_for_response(data);
		printf("resp:%2X\n",resp);
		break;
	
	default:
		printf("usage:cmd_spi_write data\n");
		break;
	}
}
MSH_CMD_EXPORT(cmd_spi_write,cmd_spi_write);


static rt_err_t configure(struct rt_spi_device *device, struct rt_spi_configuration *configuration);
static rt_uint32_t xfer(struct rt_spi_device *device, struct rt_spi_message *message);


static rt_err_t configure(struct rt_spi_device *device,
                          struct rt_spi_configuration *configuration)
{

    
    unsigned char cpol = 0;
    unsigned char cpha = 0;


    RT_ASSERT(NULL != device);
    RT_ASSERT(NULL != configuration);


    // baudrate
    if (configuration->mode & RT_SPI_CPOL)      // cpol
    {
        cpol = 1;
    }
    else
    {
        cpol = 0;
    }
    if (configuration->mode & RT_SPI_CPHA)      // cpha
    {
        cpha = 1;
    }
    else
    {
        cpha = 0;
    }
    rt_kprintf("configure: cpol:%d cpha:%d\n",cpol,cpha);
    spi_init(0b1011,cpol,cpha);


    return RT_EOK;
}

static rt_uint32_t xfer(struct rt_spi_device *device,
                        struct rt_spi_message *message)
{


    rt_kprintf("xfer:\n");
    unsigned char cs = 0;
    rt_uint32_t size = 0;
    const rt_uint8_t *send_ptr = NULL;
    rt_uint8_t *recv_ptr = NULL;
    rt_uint8_t data = 0;

    RT_ASSERT(NULL != device);
    RT_ASSERT(NULL != message);

    cs =(unsigned char )(device->parent.user_data);
    size = message->length;

    rt_kprintf("[%s] cs=%d\n", __FUNCTION__,  cs);




    // take cs
    if (message->cs_take)
    {
        spi_set_cs(cs, 0);
    }

    // 收发数据
    send_ptr = message->send_buf;
    recv_ptr = message->recv_buf;
    while (size--)
    {
        data = 0xFF;
        if (NULL != send_ptr)
        {
            data = *send_ptr++;
        }

        if (NULL != recv_ptr)
        {
            *recv_ptr++ = spi_write_for_response(data);
        }
        else
        {
            spi_write_for_response(data);
        }
    }

    // release cs
    if (message->cs_release)
    {
        spi_set_cs(cs, 1);
    }

    return message->length;
}

static struct rt_spi_ops loongson_spi_ops =
{
    .configure  = configure,
    .xfer       = xfer
};

static struct rt_spi_bus loongson_spi;

int loongson_spi_init()
{	
    rt_kprintf("spi_init\n");
	return rt_spi_bus_register(&loongson_spi,"spi",&loongson_spi_ops);
}

INIT_BOARD_EXPORT(loongson_spi_init);


#endif