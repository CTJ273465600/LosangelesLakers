

#include <rtthread.h>
#include <drivers/spi.h>
#ifdef RT_USING_DFS
#include <dfs_fs.h>

struct rt_spi_device user_spi_rom={

};

static int rt_hw_spi_flash_init()
{
    rt_kprintf("spi_flash_init\n");
    rt_spi_bus_attach_device(&user_spi_rom,"eeprom0","spi",12);//GPIO37
    if (RT_NULL == rt_sfud_flash_probe("W25Q80", "eeprom0"))
    {
        rt_kprintf("spi_flash_init_err\n");
        return -RT_ERROR;
    };
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_spi_flash_init);
int mnt_init(void)
{
    
    if (dfs_mount(RT_NULL, "/", "ram", 0, dfs_ramfs_create(rt_malloc(10*1024*1000),10*1024*1000)) == 0)
    {
        rt_kprintf("RAM file system initializated!\n");
    }
    else
    {
        rt_kprintf("RAM file system initializate failed!\n");
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);
#endif