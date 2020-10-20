/*
 * (C) Copyright 2008  Ingenic Semiconductor
 *
 *  Author: <lhhuang@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * This file contains the configuration parameters for the fuwa board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <config.h>

#define __GPIO(p, n) (32 * (p - 'A') + n)

#ifdef CONFIG_JZ_RECOVERY
#define CFG_JZ_LINUX_RECOVERY
//#define CFG_SUPPORT_RECOVERY_REG
#define CFG_SUPPORT_RECOVERY_KEY
//#define CFG_SUPPORT_RECOVERY_MISC

#endif
//#define DEBUG

#define CONFIG_MIPS32		1  /* MIPS32 CPU core */
#define CONFIG_JzRISC		1  /* JzRISC core */
#define CONFIG_LEPUS		1  /* cygnus validation board */
#define CONFIG_JZSOC		1  /* Jz SoC */
#define CONFIG_JZ4760		1  /* Jz4760 SoC */
//#define CFG_NAND_BCH_WITH_OOB	1  /* Support uImage.oob */


#ifdef CONFIG_MSC_U_BOOT
#define CONFIG_MSC_TYPE_SD //allen close for emc
#endif

#define mdelay(n) 		udelay((n)*1000)

/* memory group */
//allen add
#if defined(CONFIG_SDRAM_SDRAM)
#include "asm/jz_mem_nand_configs/SDRAM_MT48LC64M16A2.h"

#elif defined (CONFIG_SDRAM_DDR1)
#include "asm/jz_mem_nand_configs/DDR1_H5DU516ETR-E3C.h"

#elif defined (CONFIG_SDRAM_DDR2)
//#include "asm/jz_mem_nand_configs/DDR2_H5PS1G63EFR-G7C.h"
//#include "asm/jz_mem_nand_configs/DDR2_H5PS1G63EFR-Y5C.h"
#include "asm/jz_mem_nand_configs/DDR2_H5PS1G63EFR-S6C.h"

#elif defined(CONFIG_SDRAM_MDDR)
#include "asm/jz_mem_nand_configs/MDDR_K4X1G163PE-FGC6.h"
#endif

// [MAY CHANGE] NAND
//#include "asm/jz_mem_nand_configs/NAND_K9GAG08U0M.h"

#define JZ4760_NORBOOT_CFG	JZ4760_NORBOOT_8BIT	/* NOR Boot config code */

#define CFG_EXTAL			(12 * 1000000)	/* EXTAL freq: 12MHz */
#define CFG_CPU_SPEED		(528 * 1000000)	/* CPU clock */
//#define CFG_CPU_SPEED		(432 * 1000000)

#define	CFG_HZ				(CFG_EXTAL / 256) /* incrementer freq */
//#define CFG_PLL1_FRQ		(240 * 1000000) /* PLL1_FRQ */
#define CFG_PLL1_FRQ		(394 * 1000000) /* PLL1_FRQ */ //for extern codec

/* this must be included AFTER CFG_EXTAL and CFG_CPU_SPEED */
#include "jz4760_common.h"

#define CFG_UART_BASE		UART1_BASE	/* Base of the UART channel */
#define CONFIG_BAUDRATE		57600
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_MMC				1
#define CONFIG_FAT				1
#define CONFIG_SUPPORT_VFAT		1

#define CONFIG_COMMANDS		(CONFIG_CMD_DFL | \
				 CFG_CMD_MSC    | \
				 CFG_CMD_MMC    | \
				 CFG_CMD_FAT)
//#define CONFIG_BOOTP_MASK	( CONFIG_BOOTP_DEFAUL )

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	0
#define CONFIG_BOOTFILE	        "uImage"	/* file to load */

#define CONFIG_BOOTTYPE			"normal"
#define CONFIG_BOOTTYPE_REVY	"recovery"

#if defined(CONFIG_JZ4760_LCD_RG_V10)
	#define CONFIG_HWVARIANT " hwvariant=1a"
#elif defined(CONFIG_JZ4760_LCD_RG_V21)
	#define CONFIG_HWVARIANT " hwvariant=2a"
#elif defined(CONFIG_JZ4760_LCD_RG_V30)
	#define CONFIG_HWVARIANT " hwvariant=3a"
#elif defined(CONFIG_JZ4760_LCD_TM370_LN430_9)
	#define CONFIG_HWVARIANT " hwvariant=4a"
#elif defined(CONFIG_JZ4760_LCD_RG_IPS)
	#define CONFIG_HWVARIANT " hwvariant=5a"
#else
	#define CONFIG_HWVARIANT ""
#endif

#define LINUX_CMDLINE0 \
	"mem=256M console=ttyS1,57600n8 ip=off rootfstype=yaffs2 root=/dev/mtdblock2 rw"

#define LINUX_CMDLINE1 \
	"mem=192M console=ttyS1,57600n8 ip=192.168.10.125 rw rdinit=/linuxrc"

#define LINUX_CMDLINE2 \
	"mem=192M console=ttyS1,57600n8 ip=off root=/dev/ram0 rw rdinit=/init"

#define LINUX_CMDLINE3 \
	"mem=128M console=ttyS1,57600n8 ip=off rootfstype=ext3 root=/dev/mmcblk0p1 ro panic=5" CONFIG_HWVARIANT

#define LINUX_CMDLINE4 \
	"mem=192M console=ttyS1,57600n8 ip=192.168.1.125 nfsroot=192.168.1.171:/root/ingenic/nfsroot rw noinitrd"

#define LINUX_CMDLINE5 \
	"mem=192M console=ttyS1,57600n8 ip=192.168.10.125 nfsroot=192.168.10.115:/root/ingenic/nfsroot rw noinitrd"

#define LINUX_CMDLINE6 \
	"mem=128M console=ttyS1,57600n8 ip=off rootfstype=ext3 root=/dev/mmcblk1p1 ro panic=5" CONFIG_HWVARIANT

#define LINUX_CMDLINE7 \
	"mem=128M console=ttyS1,57600n8 ip=off rootfstype=vfat root=/dev/mmcblk0p1 boot=/dev/mmcblk0p1 init=/mininit-syspart rw panic=5" CONFIG_HWVARIANT

#define LINUX_CMDLINE8 \
	"mem=128M console=ttyS1,57600n8 ip=off rootfstype=vfat root=/dev/mmcblk1p1 boot=/dev/mmcblk0p1 init=/mininit-syspart rw panic=5" CONFIG_HWVARIANT

#if 0
#define LINUX_CMDLINE9 \
	"mem=128M console=ttyS1,57600n8 ip=off rootfstype=vfat root=/dev/mmcblk0p1 boot=/dev/mmcblk0p1 init=/mininit-syspart ro panic=5" CONFIG_HWVARIANT
	// "mem=128M console=ttyS1,57600n8 ip=off root=/dev/ram0 rdinit=/init boot=/dev/mmcblk0p1 rw panic=5" CONFIG_HWVARIANT
	// "mem=128M console=ttyS1,57600n8 ip=off rootfstype=vfat root=/dev/mmcblk0p1 ro panic=5" CONFIG_HWVARIANT
	// "mem=128M console=ttyS1,57600n8 ip=off root=/dev/ram0 rdinit=/init boot=/dev/mmcblk0p1 rw panic=5" CONFIG_HWVARIANT
	// "mem=128M console=ttyS1,57600n8 ip=off rootfstype=vfat root=/dev/mmcblk0p3 ro panic=5" CONFIG_HWVARIANT
	// "mem=128M console=ttyS1,57600n8 ip=off rootfstype=ext3 root=/dev/mmcblk0p1 init=/mininit-syspart ro panic=5" CONFIG_HWVARIANT
#endif

#ifdef CFG_JZ_LINUX_RECOVERY
// #define CONFIG_REVY_BOOTARGS		LINUX_CMDLINE2
// #define CONFIG_REVY_BOOTCOMMAND	"msc read 0x80600000 0x800000 0x400000;bootm"
// #define CONFIG_REVY_BOOTCOMMAND  "mmcinit;fatload mmc 1 0x80600000 uImage.bin;bootm"

#define CONFIG_REVY_BOOTARGS		LINUX_CMDLINE8
#define CONFIG_REVY_BOOTCOMMAND		"msc  read 0x80600000 0x400000 0x300000;bootm"
#endif

#define CONFIG_BOOTARGS				LINUX_CMDLINE3
#define CONFIG_BOOTCOMMAND			"msc  read 0x80600000 0x400000 0x300000;bootm"

#define CONFIG_NET_MULTI
#define CONFIG_ETHADDR				00:2a:c6:7a:ac:de    /* Ethernet address */
#define CFG_AUTOLOAD				"n"		/* No autoload */

/*
 * Serial download configuration
 *
 */
#define CONFIG_LOADS_ECHO		1	/* echo on for serial download	*/
#define CFG_LOADS_BAUD_CHANGE	1	/* allow baudrate change	*/

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP				/* undef to save memory      */
//#define	CONFIG_AUTO_COMPLETE
#define	CFG_PROMPT		"LEPUS # "	/* Monitor Command Prompt    */
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args*/

#define CFG_MALLOC_LEN		896*1024
#define CFG_BOOTPARAMS_LEN	128*1024

#define CFG_SDRAM_BASE		0x80000000     /* Cached addr */

#define CFG_INIT_SP_OFFSET	0x400000

#define	CFG_LOAD_ADDR		0x80600000     /* default load address	*/

#define CFG_MEMTEST_START	0x80100000
#define CFG_MEMTEST_END		0x80800000

#define CFG_RX_ETH_BUFFER	16	/* use 16 rx buffers on jz47xx eth */

/*
 * Configurable options for zImage if SPL is to load zImage instead of u-boot.
 */
#define CONFIG_LOAD_UBOOT	/* If it's defined, then spl load u-boot instead of zImage, and following options isn't used */
#define PARAM_BASE					0x80004000      /* The base of parameters which will be sent to kernel zImage */
#define CFG_ZIMAGE_SIZE				(2 << 20)	/* Size of kernel zImage */
#define CFG_ZIMAGE_DST				0x80100000	/* Load kernel zImage to this addr */
#define CFG_ZIMAGE_START			CFG_ZIMAGE_DST	/* Start kernel zImage from this addr	*/
#define CFG_CMDLINE					CONFIG_BOOTARGS
#define CFG_NAND_ZIMAGE_OFFS		(CFG_NAND_BLOCK_SIZE*4) /* NAND offset of zImage being loaded */
#define CFG_SPI_ZIMAGE_OFFS			(256 << 10) /* NAND offset of zImage being loaded */



#if defined(CONFIG_MSC_U_BOOT)
/*-----------------------------------------------------------------------
 *  * MSC Partition info
 *   */
/*======== Partition size ============ */
#define PTN_UBOOT_SIZE				(3* 0x100000)	/*include MBR(512bytes) before u-boot*/
#define PTN_MISC_SIZE				(1* 0x100000)
#define PTN_KERNEL_SIZE				(4* 0x100000)
#define PTN_RECOVERY_SIZE			0//(4* 0x100000)
#define PTN_SYSTEM_SIZE				(250* 0x100000) //rootfs
#define PTN_USERDATA_SIZE			(90* 0x100000)  //appfs
#define PTN_CACHE_SIZE				(30* 0x100000)	//cfg
#if 1 //max is 4g
#define PTN_VFAT_SIZE				(6718*1024)
#else //8G
#define PTN_VFAT_SIZE				(0x1000000UL)
#endif

/*======== Partition offset ============ */
#define PTN_UBOOT_OFFSET			(0)		/* 0 is MBR offset,MBR in combination with UBOOT */
#define PTN_MISC_OFFSET				(PTN_UBOOT_OFFSET+ PTN_UBOOT_SIZE)
#define PTN_KERNEL_OFFSET			(PTN_MISC_OFFSET + PTN_MISC_SIZE)
#define PTN_RECOVERY_OFFSET			(PTN_KERNEL_OFFSET + PTN_KERNEL_SIZE)
#define PTN_SYSTEM_OFFSET			(PTN_RECOVERY_OFFSET+PTN_RECOVERY_SIZE)
#define PTN_USERDATA_OFFSET			(PTN_SYSTEM_OFFSET+PTN_SYSTEM_SIZE)
#define PTN_CACHE_OFFSET			(PTN_USERDATA_OFFSET+PTN_USERDATA_SIZE)  /* optional */

/*-----------------------------------------------------------------------
 * MBR Partition info
 */
#define JZ_MBR_TABLE		/* configure the MBR below if JZ_MBR_TABLE defined*/
#define LINUX_FS_TYPE	0x83
#define VFAT_FS_TYPE	0x0B
/*======== Partition table ============ */
#define MBR_P1_OFFSET 	PTN_SYSTEM_OFFSET
#define MBR_P1_SIZE 	PTN_SYSTEM_SIZE
#define MBR_P1_TYPE 	LINUX_FS_TYPE

#define MBR_P2_OFFSET 	PTN_USERDATA_OFFSET
#define MBR_P2_SIZE 	PTN_USERDATA_SIZE
#define MBR_P2_TYPE 	LINUX_FS_TYPE

#define MBR_P3_OFFSET 	PTN_CACHE_OFFSET
#define MBR_P3_SIZE 	PTN_CACHE_SIZE
#define MBR_P3_TYPE 	LINUX_FS_TYPE

#define MBR_P4_OFFSET 	(PTN_CACHE_OFFSET+PTN_CACHE_SIZE)
#define MBR_P4_SIZE 	PTN_VFAT_SIZE
#define MBR_P4_TYPE 	VFAT_FS_TYPE

#endif

/*-----------------------------------------------------------------------
 * Environment
 *----------------------------------------------------------------------*/
#define CFG_ENV_IS_IN_MSC       1

#define CFG_NAND_BCH_BIT        8               /* Specify the hardware BCH algorithm for 4760 (4|8) */
#define CFG_NAND_ECC_POS        24               /* Ecc offset position in oob area, its default value is 3 if it isn't defined. */
#define CFG_NAND_SMCR1          0x0d444400      /* 0x0fff7700 is slowest */
#define CFG_NAND_USE_PN         0               /* Use PN in jz4760 for TLC NAND */
#define CFG_NAND_BACKUP_NUM     1               /* TODO */

#define CFG_MAX_NAND_DEVICE     1
#define NAND_MAX_CHIPS          1
#define CFG_NAND_BASE           0xBA000000
#define NAND_ADDR_OFFSET        0x00800000
#define NAND_CMD_OFFSET         0x00400000
#define CFG_NAND_SELECT_DEVICE  1       /* nand driver supports mutipl. chips   */

/*
 * IPL (Initial Program Loader, integrated inside CPU)
 * Will load first 8k from NAND (SPL) into cache and execute it from there.
 *
 * SPL (Secondary Program Loader)
 * Will load special U-Boot version (NUB) from NAND and execute it. This SPL
 * has to fit into 8kByte. It sets up the CPU and configures the SDRAM
 * controller and the NAND controller so that the special U-Boot image can be
 * loaded from NAND to SDRAM.
 *
 * NUB (NAND U-Boot)
 * This NAND U-Boot (NUB) is a special U-Boot version which can be started
 * from RAM. Therefore it mustn't (re-)configure the SDRAM controller.
 *
 */
#define CFG_NAND_U_BOOT_DST		0xa0100000	/* Load NUB to this addr	*/
#define CFG_NAND_U_BOOT_START	0x80100000 /* Start NUB from this addr	*/

/*
 * Define the partitioning of the NAND chip (only RAM U-Boot is needed here)
 */
#define CFG_NAND_U_BOOT_OFFS	(CFG_NAND_BLOCK_SIZE * (CFG_NAND_BACKUP_NUM+1))	/* Offset to U-Boot image */

/* Size of U-Boot image */
#if CFG_NAND_BLOCK_SIZE > (512 << 10)
#define CFG_NAND_U_BOOT_SIZE    CFG_NAND_BLOCK_SIZE
#else
#define CFG_NAND_U_BOOT_SIZE	(512 << 10)
#endif

/*
 * IPL (Initial Program Loader, integrated inside CPU)
 * Will load first 8k from MSC (SPL) into cache and execute it from there.
 *
 * SPL (Secondary Program Loader)
 * Will load special U-Boot version (MSUB) from MSC and execute it. This SPL
 * has to fit into 8kByte. It sets up the CPU and configures the SDRAM
 * controller and the MSC controller so that the special U-Boot image can be
 * loaded from MSC to SDRAM.
 *
 * MSUB (MMC/SD U-Boot)
 * This MSC U-Boot (MSUB) is a special U-Boot version which can be started
 * from RAM. Therefore it mustn't (re-)configure the SDRAM controller.
 *
 */
#define CFG_MSC_U_BOOT_DST		0x80100000	/* Load MSUB to this addr	 */
#define CFG_MSC_U_BOOT_START	CFG_MSC_U_BOOT_DST /* Start MSUB from this addr */

/*
 * Define the partitioning of the MMC/SD card (only RAM U-Boot is needed here)
 */
#define CFG_MSC_U_BOOT_OFFS	(16 << 10)	/* Offset to RAM U-Boot image	*/

// Size of RAM U-Boot image
#define CFG_MSC_U_BOOT_SIZE	(512 << 12)//(512 << 10)

#define CFG_MSC_BLOCK_SIZE	512

#ifdef CFG_ENV_IS_IN_MSC
#define CFG_ENV_SIZE		(CFG_MSC_BLOCK_SIZE<<1)
#define CFG_ENV_OFFSET		((CFG_MSC_BLOCK_SIZE * 16) + CFG_MSC_U_BOOT_SIZE + (CFG_MSC_BLOCK_SIZE * 16))	/* environment starts here  */

#ifdef CFG_JZ_LINUX_RECOVERY
#define CONFIG_DEFAULT_ENV_SIZE 	CFG_ENV_SIZE
#define CFG_ENV_REVY_OFFSET			(CFG_ENV_OFFSET + CFG_ENV_SIZE)	/* environment starts here  */
#endif
#endif


/*-----------------------------------------------------------------------
 * SPI NOR FLASH configuration
 */
#define CFG_SPI_MAX_FREQ		1000000
#define CFG_SPI_U_BOOT_DST		0x80100000	/* Load NUB to this addr	*/
#define CFG_SPI_U_BOOT_START	CFG_SPI_U_BOOT_DST
#define CFG_SPI_U_BOOT_OFFS		(8 << 10)
#define CFG_SPI_U_BOOT_SIZE		(256 << 10)

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xB8000000 /* Flash Bank #1 */

/* The following #defines are needed to get flash environment right */
#define	CFG_MONITOR_BASE	0xb8000000
#define	CFG_MONITOR_LEN		(256*1024)  /* Reserve 256 kB for Monitor */

#define CFG_FLASH_BASE		PHYS_FLASH_1
/* Environment settings */
#ifdef CFG_ENV_IS_IN_FLASH

#define CFG_ENV_SECT_SIZE	0x20000 /* Total Size of Environment Sector */
#define CFG_ENV_SIZE		CFG_ENV_SECT_SIZE
#endif
#define CFG_ENV_ADDR		0xB8040000

#define CFG_DIRECT_FLASH_TFTP	1	/* allow direct tftp to flash */

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		16384
#define CFG_ICACHE_SIZE		16384
#define CFG_CACHELINE_SIZE	32

/*======================================================================
 * GPIO
 */

// #define CONFIG_LCD

#ifdef CONFIG_LCD		 /* LCD support */
// #define CONFIG_JZ4760_LCD_SNK
//#define CONFIG_JZ4760_SLCD_SNK
#define LCD_BPP   LCD_COLOR16
#endif

#ifdef  CONFIG_JZ4760_SLCD_SNK
#define CONFIG_FB_JZ4750_SLCD
#endif
#define LOW_BATTERY_DATA		3400
#define WARN_BATTERY_DATA		3500
#define NORMAL_BATTERY_DATA		3700
#define BATTERY_FULL_VALUE		4500//to app
#define BATTERY_USB_OFFSET		110

#define GPIO_LCD_PWM			__GPIO('E', 1)	// GPE1 PWM1
#define GPIO_LCD_VCC_EN			__GPIO('E', 25)	// ln430_9 no use,but no del
#define GPIO_CHARG_ON			__GPIO('B', 29)	// GPB30 PWM1 */
#define GPIO_USB_DETE			__GPIO('D', 7)	// __GPIO('B', 21)

#define GPIO_LCD_POWER_N		__GPIO('B', 31)	// GPB31
#define GPIO_LCD_DISP_N			__GPIO('F', 6)	// LCD_DISP_N use for lcd reset
#define GPIO_KEY_WAKEUP			__GPIO('A', 30)	// GPA32 WAKEUP
#define GPIO_KEY_VOLUME_INC		__GPIO('C', 31)	// GPC31  sw1
#define GPIO_KEY_VOLUME_DEC		__GPIO('C', 29)	// GPC29  sw3
#define GPIO_KEY_MENU			__GPIO('D', 27)	// GPD27
#define GPIO_KEY_BOOT			__GPIO('D', 17)	// GPD17 BOOT_SEL0

#define UMIDO_KEY_R				__GPIO('D', 24)
#define UMIDO_KEY_X				__GPIO('E', 7)

#define GPIO_SD0_VCC_EN_N		__GPIO('F', 9)	// GPF9
// #define GPIO_SD0_CD_N		__GPIO('B', 22)	// GPB22
#define GPIO_SD0_CD_N			__GPIO('F', 0)	// GPF00
#define GPIO_SD0_WP_N			__GPIO('F', 4)	// GPF4
#define GPIO_SD1_VCC_EN_N		__GPIO('E', 9)	// GPE9
#define GPIO_SD1_CD_N			__GPIO('A', 28)	// GPA28

#define UBOOT_SEL_REVY_KEY1		UMIDO_KEY_R
#define UBOOT_SEL_REVY_KEY2		UMIDO_KEY_X
#define UBOOT_SEL_REVY_KEY3		UMIDO_KEY_R

#endif	/* __CONFIG_H */
