/*
 * Copyright (C) 2007 Ingenic Semiconductor Inc.
 * Author: Peter <jlwei@ingenic.cn>
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
#include <config.h>
#include <common.h>
#include <asm/io.h>

#if defined(CONFIG_JZ4760B)
#include <asm/jz4760b.h>
#endif

/*
 * External routines
 */
extern void flush_cache_all(void);
extern int serial_init(void);
extern void serial_puts(const char *s);
extern void sdram_init(void);
extern void pll_init(void);

#define u32 unsigned int
#define u16 unsigned short
#define u8 unsigned char
static int highcap = 0;

/*
 * GPIO definition
 */
#define MMC_IRQ_MASK()				\
do {						\
	REG_MSC_IMASK = 0xffff;			\
	REG_MSC_IREG = 0xffff;			\
} while (0)

#if 0
static void mudelay(unsigned int usec)
{
    //unsigned int i = usec * (336000000 / 2000000);
	unsigned int i = usec  << 7;
    __asm__ __volatile__ (
        "\t.set noreorder\n"
        "1:\n\t"
        "bne\t%0, $0, 1b\n\t"
        "addi\t%0, %0, -1\n\t"
        ".set reorder\n"
        : "=r" (i)
        : "0" (i)
    );

}

static void sd_mdelay(int sdelay)
{
    mudelay(sdelay * 1000);
}
#endif

/* Stop the MMC clock and wait while it happens */
static int jz_mmc_stop_clock(void)
{
	int timeout = 1000;
	int wait = 12; /* 1 us */

	REG_MSC_STRPCL = MSC_STRPCL_CLOCK_CONTROL_STOP;
	while (timeout && (REG_MSC_STAT & MSC_STAT_CLK_EN)) {
		timeout--;
		if (timeout == 0) {
			return -1;
		}
		wait = 12;
		while (wait--)
			;
	}
	return 0;
}

static u8 * mmc_cmd(u16 cmd, unsigned int arg, unsigned int cmdat, u16 rtype)
{
	static u8 resp[20];
	u32 timeout = 0x3fffff;
	int words, i;

	jz_mmc_stop_clock();
	REG_MSC_CMD   = cmd;
	REG_MSC_ARG   = arg;
	REG_MSC_CMDAT = cmdat;

	REG_MSC_IMASK = ~MSC_IMASK_END_CMD_RES;
	REG_MSC_STRPCL = MSC_STRPCL_CLOCK_CONTROL_START | MSC_STRPCL_START_OP;

	while (timeout-- && !(REG_MSC_STAT & MSC_STAT_END_CMD_RES))
		;

	REG_MSC_IREG = MSC_IREG_END_CMD_RES;

	switch (rtype) {
	case MSC_CMDAT_RESPONSE_R1:
		case MSC_CMDAT_RESPONSE_R3:
			words = 3;
			break;
		case MSC_CMDAT_RESPONSE_R2:
			words = 8;
			break;
		default:
			return 0;
	}

	for (i = words-1; i >= 0; i--) {
		u16 res_fifo = REG_MSC_RES;
		int offset = i << 1;

		resp[offset] = ((u8 *)&res_fifo)[0];
		resp[offset+1] = ((u8 *)&res_fifo)[1];
	}
	return resp;
}

int mmc_block_readm(u32 src, u32 num, u8 *dst)
{
	u8 *resp;
	u32 stat, timeout, data, cnt, wait, nob;

	resp = mmc_cmd(16, 0x200, 0x401, MSC_CMDAT_RESPONSE_R1);
	REG_MSC_BLKLEN = 0x200;
	REG_MSC_NOB = num / 512;
#if  1 //old is ok 20160727
	if (highcap) 
		resp = mmc_cmd(18, src, 0x409, MSC_CMDAT_RESPONSE_R1); // for sdhc card
	else
		resp = mmc_cmd(18, src * 512, 0x409, MSC_CMDAT_RESPONSE_R1);
#else //hifi-emc
	if (highcap) 
		resp = mmc_cmd(18, src, 0x10409, MSC_CMDAT_RESPONSE_R1); // for sdhc card
	else
		resp = mmc_cmd(18, src * 512, 0x10409, MSC_CMDAT_RESPONSE_R1);

#endif
	nob  = num / 512;

	for (; nob >= 1; nob--) {
		timeout = 0x7ffffff;
		while (timeout) {
			timeout--;
			stat = REG_MSC_STAT;
		
			if (stat & MSC_STAT_TIME_OUT_READ) {
				//serial_puts("\n TIME_OUT_READ\n\n");
				return -1;
			}
			else if (stat & MSC_STAT_CRC_READ_ERROR) {
				//serial_puts("\n CRC_READ_ERROR\n\n");
				return -1;
			}
			else if (!(stat & MSC_STAT_DATA_FIFO_EMPTY)) {
				/* Ready to read data */
				break;
			}

			wait = 120;
			while (wait--)
				;
		}
		if (!timeout) {
			//serial_puts("read timeout\n");
			return -1;
		}

		/* Read data from RXFIFO. It could be FULL or PARTIAL FULL */
		cnt = 128;
		while (cnt) {
			while (cnt && (REG_MSC_STAT & MSC_STAT_DATA_FIFO_EMPTY))
				;
			cnt --;
			data = REG_MSC_RXFIFO;
			{
				*dst++ = (u8)(data >> 0);
				*dst++ = (u8)(data >> 8);
				*dst++ = (u8)(data >> 16);
				*dst++ = (u8)(data >> 24);
			}
		}
	}
	
#if 1  //old is ok 20160727
	resp = mmc_cmd(12, 0, 0x41, MSC_CMDAT_RESPONSE_R1);
#else //hifi-emc
	while(!(REG_MSC_STAT & (1 << 31)));
#endif
	jz_mmc_stop_clock();

	return 0;
}

#ifdef CONFIG_MSC_TYPE_SD
static void sd_found(void)
{
	int rca;
	int retries, wait;
	u8 *resp;
	unsigned int cardaddr;
	//serial_puts("SD card found!\n");

	resp = mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
	retries = 100;
	while (retries-- && resp && !(resp[4] & 0x80)) {
		resp = mmc_cmd(55, 0, 0x1, MSC_CMDAT_RESPONSE_R1);
		resp = mmc_cmd(41, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
		
		wait = 336000000; // sd_mdelay(10);
		while (wait--);
	}
	
#if 0
	if (resp[4] & 0x80)
		serial_puts("init ok\n");
	else
		serial_puts("init fail\n");
#endif

	/* try to get card id */
	resp = mmc_cmd(2, 0, 0x2, MSC_CMDAT_RESPONSE_R2);
	resp = mmc_cmd(3, 0, 0x6, MSC_CMDAT_RESPONSE_R1);
	cardaddr = (resp[4] << 8) | resp[3];
	rca = cardaddr << 16;

	resp = mmc_cmd(9, rca, 0x2, MSC_CMDAT_RESPONSE_R2);
	highcap = (resp[14] & 0xc0) >> 6;
	REG_MSC_CLKRT = 2;
	resp = mmc_cmd(7, rca, 0x41, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(55, rca, 0x1, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(6, 0x2, 0x41, MSC_CMDAT_RESPONSE_R1);

}
#else 

/* init mmc/sd card we assume that the card is in the slot */
int  mmc_found(void)
{
	int retries, wait;
	u8 *resp;

//	serial_puts("MMC\n");
	resp = mmc_cmd(1, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
	retries = 1000;
	while (retries-- && resp && !(resp[4] & 0x80)) {
		resp = mmc_cmd(1, 0x40300000, 0x3, MSC_CMDAT_RESPONSE_R3);
		wait = 120000; // sd_mdelay(10);
		while (wait--);
	}

	//sd_mdelay(10);

	if ((resp[4] & 0x80 )== 0x80)
		serial_puts(" ok@\n");
	else
		serial_puts(" fail\n");

	if((resp[4] & 0x60 ) == 0x40)
		highcap = 1;
	else
		highcap =0;
	/* try to get card id */
	resp = mmc_cmd(2, 0, 0x2, MSC_CMDAT_RESPONSE_R2);
	resp = mmc_cmd(3, 0x10, 0x1, MSC_CMDAT_RESPONSE_R1);

	REG_MSC_CLKRT = 1;	/* 16/1 MHz */
	resp = mmc_cmd(7, 0x10, 0x1, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(6, 0x3b70101, 0x441, MSC_CMDAT_RESPONSE_R1);

	return 0;
}
#endif

int  mmc_init(void)
{
	int retries, wait;
	u8 *resp;

//	REG_CPM_MSCCDR = 13;
//	REG_CPM_CPCCR |= CPM_CPCCR_CE;

	__gpio_as_msc();
	__msc_reset();

	MMC_IRQ_MASK();	
	REG_MSC_CLKRT = 6;    //187k
	REG_MSC_RDTO = 0xffffffff;
	
	/* just for reading and writing, suddenly it was reset, and the power of sd card was not broken off */
	resp = mmc_cmd(12, 0, 0x41, MSC_CMDAT_RESPONSE_R1);

	/* reset */
	resp = mmc_cmd(0, 0, 0x80, 0);
	resp = mmc_cmd(8, 0x1aa, 0x1, MSC_CMDAT_RESPONSE_R1);
	resp = mmc_cmd(55, 0, 0x1, MSC_CMDAT_RESPONSE_R1);
	if(!(resp[0] & 0x20) && (resp[5] != 0x37)) 
	{ 
		//serial_puts("\n\nMMC card found!\n\n");	
		resp = mmc_cmd(1, 0x40ff8000, 0x3, MSC_CMDAT_RESPONSE_R3);
		retries = 1000;
		while (retries-- && resp && !(resp[4] & 0x80)) {
			resp = mmc_cmd(1, 0x40300000, 0x3, MSC_CMDAT_RESPONSE_R3);
			wait = 120000; // mdelay(10);
			while (wait--);
		}
		
		if((resp[4] & 0x60 ) == 0x40)
		{
		    //serial_puts("is highcap \n");
			highcap = 1;
		}
		else
			highcap =0;

		/* try to get card id */
		resp = mmc_cmd(2, 0, 0x2, MSC_CMDAT_RESPONSE_R2);
		resp = mmc_cmd(3, 0x10, 0x1, MSC_CMDAT_RESPONSE_R1);
		REG_MSC_CLKRT = 1;	/* 16/1 MHz */
		resp = mmc_cmd(7, 0x10, 0x1, MSC_CMDAT_RESPONSE_R1);
		resp = mmc_cmd(6, 0x3b70101, 0x441, MSC_CMDAT_RESPONSE_R1);

	}
	else
		sd_found();

	return 0;
}

/*
 * Load kernel image from MMC/SD into RAM
 */
static int mmc_load(int uboot_size, u8 *dst)
{
	mmc_init();
	mmc_block_readm(32, uboot_size, dst);

	return 0;
}

void spl_boot(void)
{
	void (*uboot)(void);

	/*
	 * Init hardware
	 */
	__cpm_start_mdma();
	__cpm_start_ddr();
	/* enable mdmac's clock */
	REG_MDMAC_DMACKES = 0x3;
	REG_MDMAC_DMACKE = 0x3;

	__gpio_as_uart1();
	__cpm_start_uart1();
	serial_init();

	serial_puts("\n\nMSC Secondary Program Loader\n");

#ifndef CONFIG_FPGA
	pll_init();
#endif

	sdram_init();

	/*
	 * Load U-Boot image from NAND into RAM
	 */
	mmc_load(CFG_MSC_U_BOOT_SIZE, (uchar *)CFG_MSC_U_BOOT_DST);

	uboot = (void (*)(void))CFG_NAND_U_BOOT_START;

	serial_puts("Starting U-Boot ...\n");

	/*
	 * Flush caches
	 */
	flush_cache_all();

	/*
	 * Jump to U-Boot image
	 */
	(*uboot)();
}
