/*
 * (C) Copyright 2006
 * Ingenic Semiconductor, <jlwei@ingenic.cn>
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

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/jz4760b.h>

extern void me_battery_init(void);
extern void sadc_start_pbat(void);
extern void sadc_stop_pbat(void);
extern void me_do_hibernate(void);

static void gpio_init(void)
{
	/* For ethernet data line init */
	__gpio_as_nand_16bit(1);


	/*
	 * Initialize UART1 pins
	 */
#if CFG_UART_BASE == UART0_BASE
	__gpio_as_uart0();
#elif CFG_UART_BASE == UART1_BASE
	__gpio_as_uart1();
#elif CFG_UART_BASE == UART2_BASE
	__gpio_as_uart2();
#else /* CFG_UART_BASE == UART1_BASE */
	__gpio_as_uart3();
#endif
}

//----------------------------------------------------------------------
// board early init routine
void board_early_init(void)
{
#if 0
#define CS2  (32*0+23)
	__gpio_as_output(CS2);
	while(1) {
		int i;
		__gpio_set_pin(CS2);
		i=480000;
		while(i--);
		__gpio_clear_pin(CS2);
		i=480000;
		while(i--);
	}
#endif
	gpio_init();
}


//----------------------------------------------------------------------
// U-Boot common routines

int checkboard (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	printf("Board: Ingenic LEPUS (CPU Speed %d MHz)\n",
	       gd->cpu_clk/1000000);

	return 0; /* success */
}
//----------------------------------------------------
//allen add (refer to snk uboot)

int usb_detect(void)
{
	__gpio_as_input(GPIO_USB_DETE);
	__gpio_disable_pull(GPIO_USB_DETE);

	if(__gpio_get_pin(GPIO_USB_DETE))
		return 1;
	else
		return 0;
}

int get_battery_mv(void)
{
	unsigned int timeout = 0x3fff;

	//return WARN_BATTERY_DATA; //debug test

	me_battery_init();
	sadc_start_pbat();
	while((REG_SADC_STATE & SADC_STATE_PBATRDY ) == 0 && (--timeout)){
		;
	}
	if (timeout < 2){
		serial_puts("LN430 read vbat timeout!\n");
		return 0;
	}
	unsigned int val = REG_SADC_BATDAT;

	val = (val * 2500 / 4096) * 4;//allen mod

	printf("read vbat value is %d\n",val);

	sadc_stop_pbat();

	return val;

}

void power_off_ub(void)
{
	 me_do_hibernate();
}

