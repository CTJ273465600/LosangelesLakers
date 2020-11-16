#include <rtthread.h>
#include <drivers/pin.h>
#ifdef RT_USING_PIN
static int cmd_gpio_mode(int argc,char* argv[])
{
  //rt_kprintf("gpio_read %d\n",pin);
  int pin,val;
  pin=strtoul(argv[1], NULL, 0);
  val=strtoul(argv[2], NULL, 0);
  rt_pin_mode(pin, val);
}
MSH_CMD_EXPORT(cmd_gpio_mode,init);

static int cmd_gpio_read(int argc,char* argv[])
{
  //rt_kprintf("gpio_read %d\n",pin);
  int pin;
  pin=strtoul(argv[1], NULL, 0);
  rt_kprintf("%d\n",rt_pin_read(pin));
}
MSH_CMD_EXPORT(cmd_gpio_read,read);

static void cmd_gpio_write(int argc,char* argv[])
{
  //rt_kprintf("gpio_write %d,%d\n",pin,val);
  int pin,val;
  pin=strtoul(argv[1], NULL, 0);
  val=strtoul(argv[2], NULL, 0);
  rt_pin_write(pin, val);

}
MSH_CMD_EXPORT(cmd_gpio_write,write);
#endif