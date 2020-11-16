#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 30
#define RT_ALIGN_SIZE 8
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 16384
#define RT_DEBUG

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 256
#define RT_CONSOLE_DEVICE_NAME "uart"
#define RT_VER_NUM 0x40003
#define ARCH_CPU_64BIT
#define ARCH_MIPS64

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 16384
#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 16384000
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 10
#define DFS_FILESYSTEM_TYPES_MAX 10
#define DFS_FD_MAX 16
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 936
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 9
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS
#define RT_USING_DFS_RAMFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 16384
#define RT_SYSTEM_WORKQUEUE_PRIORITY 5
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_PWM
#define RT_USING_RTC
#define RT_USING_SPI
#define RT_USING_SPI_MSD
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE
#define RT_SFUD_SPI_MAX_HZ 50000000

/* Using USB */


/* POSIX layer and C standard library */

#define RT_USING_LIBC
#define RT_USING_POSIX

/* Network */

/* Socket abstraction layer */

#define RT_USING_SAL

/* protocol stack implement */

#define SAL_USING_LWIP
#define SAL_USING_POSIX

/* Network interface device */

#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0

/* light weight TCP/IP stack */

#define RT_USING_LWIP
#define RT_USING_LWIP202
#define RT_LWIP_MEM_ALIGNMENT 8
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
#define RT_LWIP_SNMP
#define RT_LWIP_DNS

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "169.254.231.200"
#define RT_LWIP_GWADDR "169.254.231.245"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
#define RT_LWIP_RAW
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 8196
#define RT_LWIP_TCP_WND 8196
#define RT_LWIP_TCPTHREAD_PRIORITY 5
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 32
#define RT_LWIP_TCPTHREAD_STACKSIZE 16384
#define RT_LWIP_ETHTHREAD_PRIORITY 5
#define RT_LWIP_ETHTHREAD_STACKSIZE 16384
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 32
#define RT_LWIP_REASSEMBLY_FRAG
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
#define LWIP_SO_LINGER 0
#define LWIP_NETIF_LOOPBACK 0
#define RT_LWIP_STATS
#define RT_LWIP_USING_PING

/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread MIPS CPU */


/* RT-Thread online packages */

/* IoT - internet of things */

#define PKG_USING_WEBCLIENT
#define WEBCLIENT_USING_SAMPLES
#define WEBCLIENT_USING_FILE_DOWMLOAD
#define WEBCLIENT_NOT_USE_TLS
#define PKG_USING_WEBCLIENT_V212
#define PKG_WEBCLIENT_VER_NUM 0x20102
#define PKG_USING_WEBNET
#define WEBNET_PORT 80
#define WEBNET_CONN_MAX 16
#define WEBNET_ROOT "/"

/* Select supported modules */

#define WEBNET_USING_AUTH
#define WEBNET_USING_CGI
#define WEBNET_USING_ASP
#define WEBNET_USING_SSI
#define WEBNET_USING_INDEX
#define WEBNET_USING_ALIAS
#define WEBNET_USING_UPLOAD
#define WEBNET_CACHE_LEVEL 0
#define WEBNET_USING_SAMPLES
#define PKG_USING_WEBNET_V202

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */

#define PKG_USING_NETUTILS
#define PKG_NETUTILS_TFTP
#define PKG_USING_NETUTILS_V120

/* IoT Cloud */


/* security packages */


/* language packages */

#define PKG_USING_MICROPYTHON

/* Hardware Module */

#define MICROPYTHON_USING_MACHINE_SPI
#define MICROPYTHON_USING_MACHINE_UART
#define MICROPYTHON_USING_MACHINE_PWM
#define MICROPYTHON_USING_NETWORK

/* System Module */


/* Tools Module */

#define MICROPYTHON_USING_CMATH
#define MICROPYTHON_USING_UHEAPQ

/* Network Module */


/* User Extended Module */

#define PKG_MICROPYTHON_HEAP_SIZE 8192
#define PKG_USING_MICROPYTHON_LATEST_VERSION
#define PKG_MICROPYTHON_VER_NUM 0x99999

/* multimedia packages */


/* tools packages */


/* system packages */

#define PKG_USING_LWEXT4
#define RT_USING_DFS_LWEXT4
#define PKG_USING_LWEXT4_LATEST_VERSION

/* peripheral libraries and drivers */


/* miscellaneous packages */

#define PKG_USING_OPTPARSE
#define PKG_USING_OPTPARSE_V100

/* samples: kernel and components samples */

#define PKG_USING_VI
#define VI_MAX_LEN 4096
#define VI_ENABLE_COLON
#define VI_ENABLE_YANKMARK
#define VI_ENABLE_SEARCH
#define VI_ENABLE_DOT_CMD
#define VI_ENABLE_READONLY
#define VI_ENABLE_SETOPTS
#define VI_ENABLE_SET
#define VI_ENABLE_VI_ASK_TERMINAL
#define VI_ENABLE_UNDO
#define VI_ENABLE_UNDO_QUEUE
#define VI_UNDO_QUEUE_MAX 256
#define PKG_USING_VI_LATEST_VERSION
#define SOC_LS2K1000

#endif
