/*
 * linux/drivers/video/jz4750_lcd.h -- Ingenic Jz4750 On-Chip LCD frame buffer device
 *
 * Copyright (C) 2005-2008, Ingenic Semiconductor Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __JZ4760_LCD_H__
#define __JZ4760_LCD_H__

//#include <asm/io.h>


#define NR_PALETTE	256
#define PALETTE_SIZE	(NR_PALETTE*2)

/* use new descriptor(8 words) */
struct jz4750_lcd_dma_desc {
	unsigned int next_desc; 	/* LCDDAx */
	unsigned int databuf;   	/* LCDSAx */
	unsigned int frame_id;  	/* LCDFIDx */
	unsigned int cmd; 		/* LCDCMDx */
	unsigned int offsize;       	/* Stride Offsize(in word) */
	unsigned int page_width; 	/* Stride Pagewidth(in word) */
	unsigned int cmd_num; 		/* Command Number(for SLCD) */
	unsigned int desc_size; 	/* Foreground Size */
};

struct jz4750lcd_panel_t {
	unsigned int cfg;	/* panel mode and pin usage etc. */
	unsigned int slcd_cfg;	/* Smart lcd configurations */
	unsigned int ctrl;	/* lcd controll register */
	unsigned int w;		/* Panel Width(in pixel) */
	unsigned int h;		/* Panel Height(in line) */
	unsigned int fclk;	/* frame clk */
	unsigned int hsw;	/* hsync width, in pclk */
	unsigned int vsw;	/* vsync width, in line count */
	unsigned int elw;	/* end of line, in pclk */
	unsigned int blw;	/* begin of line, in pclk */
	unsigned int efw;	/* end of frame, in line count */
	unsigned int bfw;	/* begin of frame, in line count */
};


struct jz4750lcd_fg_t {
	int bpp;	/* foreground bpp */
	int x;		/* foreground start position x */
	int y;		/* foreground start position y */
	int w;		/* foreground width */
	int h;		/* foreground height */
};

struct jz4750lcd_osd_t {
	unsigned int osd_cfg;	/* OSDEN, ALHPAEN, F0EN, F1EN, etc */
	unsigned int osd_ctrl;	/* IPUEN, OSDBPP, etc */
	unsigned int rgb_ctrl;	/* RGB Dummy, RGB sequence, RGB to YUV */
	unsigned int bgcolor;	/* background color(RGB888) */
	unsigned int colorkey0;	/* foreground0's Colorkey enable, Colorkey value */
	unsigned int colorkey1; /* foreground1's Colorkey enable, Colorkey value */
	unsigned int alpha;	/* ALPHAEN, alpha value */
	unsigned int ipu_restart; /* IPU Restart enable, ipu restart interval time */

#define FG_NOCHANGE 		0x0000
#define FG0_CHANGE_SIZE 	0x0001
#define FG0_CHANGE_POSITION 	0x0002
#define FG1_CHANGE_SIZE 	0x0010
#define FG1_CHANGE_POSITION 	0x0020
#define FG_CHANGE_ALL 		( FG0_CHANGE_SIZE | FG0_CHANGE_POSITION | \
				  FG1_CHANGE_SIZE | FG1_CHANGE_POSITION )
	int fg_change;
	struct jz4750lcd_fg_t fg0;	/* foreground 0 */
	struct jz4750lcd_fg_t fg1;	/* foreground 1 */
};

struct jz4750lcd_info {
	struct jz4750lcd_panel_t panel;
	struct jz4750lcd_osd_t osd;
};


/* Jz LCDFB supported I/O controls. */
#define FBIOSETBACKLIGHT	0x4688 /* set back light level */
#define FBIODISPON		0x4689 /* display on */
#define FBIODISPOFF		0x468a /* display off */
#define FBIORESET		0x468b /* lcd reset */
#define FBIOPRINT_REG		0x468c /* print lcd registers(debug) */
#define FBIOROTATE		0x46a0 /* rotated fb */
#define FBIOGETBUFADDRS		0x46a1 /* get buffers addresses */
#define FBIO_GET_MODE		0x46a2 /* get lcd info */
#define FBIO_SET_MODE		0x46a3 /* set osd mode */
#define FBIO_DEEP_SET_MODE	0x46a4 /* set panel and osd mode */
#define FBIO_MODE_SWITCH	0x46a5 /* switch mode between LCD and TVE */
#define FBIO_GET_TVE_MODE	0x46a6 /* get tve info */
#define FBIO_SET_TVE_MODE	0x46a7 /* set tve mode */

/*
 * LCD panel specific definition
 */

#if defined(CONFIG_JZ4760_LCD_TM370_LN430_9)
	#define SPEN		(32*4+0)       /*LCD_CS*/
	#define SPCK		(32*3+11)       /*LCD_SCL*/
	#define SPDA		(32*4+2)       /*LCD_SDA*/
	#define LCD_RET 	(32*4+4)

#define __spi_send_value(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
	unsigned char cmd_dat=0; \
	cmd_dat =reg; \
	value = val; \
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	if(cmd_dat) \
		value |= 1<<8; \
	else \
		value &= ~(1<<8); \
	for(no=0;no<9;no++) \
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x100)==0x100) \
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)

#define spi_send_cmd(cmd)     __spi_send_value(0,cmd)
#define spi_send_data(data)   __spi_send_value(1,data)
#define __lcd_special_pin_init() \
do { \
}while (0)

#define __lcd_special_on() \
do { \
	printf("\n LN430_9 lcd\n"); \
} while (0)

#define __lcd_special_off() \
do { \
	__gpio_as_output(GPIO_LCD_PWM); \
	__gpio_clear_pin(GPIO_LCD_PWM); \
} while (0)

#endif /* CONFIG_JZ4760_LCD_TM370_LN430_9 */


#if defined(CONFIG_JZ4760_LCD_SNK)

#define SPEN		(32*4+0)       /*LCD_CS*/
#define SPCK		(32*3+11)       /*LCD_SCL*/
#define SPDA		(32*4+2)       /*LCD_SDA*/
#define LCD_RET 	(32*4+4)

#define __spi_writ_bit16(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
\
	value=((reg<<8)|(val&0xFF));	\
\
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	for(no=0;no<16;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x8000)==0x8000)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)

#define __spi_send_value(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
	unsigned char cmd_dat=0; \
	cmd_dat =reg; \
	value = val; \
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	if(cmd_dat)\
		value |= 1<<8;     \
	else \
		value &= ~(1<<8);  \
	for(no=0;no<9;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x100)==0x100)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)

#define spi_send_cmd(cmd)     __spi_send_value(0,cmd)
#define spi_send_data(data)   __spi_send_value(1,data)
//-------------------------------------------------------------

#define __lcd_special_pin_init() \
 do { \
	__gpio_as_output(SPEN); /* use SPDA */	\
	__gpio_as_output(SPCK); /* use SPCK */	\
	__gpio_as_output(SPDA); /* use SPDA */	\
	__gpio_as_output(LCD_RET);              \
	udelay(50);	                            \
	__gpio_clear_pin(LCD_RET);              \
	mdelay(100);				    \
	__gpio_set_pin(LCD_RET);		\
 }while (0)

#if 1 //auo
 #if 1 //ao-8
#define __lcd_special_on()   \
  do { \
	 printf("\n auo-3.0lcd \n"); \
	  __spi_writ_bit16(0x05,0x34);	   \
	  mdelay(5); \
	  __spi_writ_bit16(0x05,0x74); \
	  __spi_writ_bit16(0x05,0x75); \
	  mdelay(10); \
  } while (0)
  #else
#define __lcd_special_on()   \
 do { \
 	printf("\n auo-ILI8965B ... \n"); \
__spi_writ_bit16(0x70, 0x06); \
__spi_writ_bit16(0x05, 0x81);   \
mdelay(48); \
__spi_writ_bit16(0x70, 0x07);   \
__spi_writ_bit16(0x06, 0x08);    \
__spi_writ_bit16(0x01, 0x59);   \
__spi_writ_bit16(0x03, 0x10);  \
__spi_writ_bit16(0x04, 0x10);   \
__spi_writ_bit16(0x08, 0x20);   \
__spi_writ_bit16(0x0A, 0x84);   \
__spi_writ_bit16(0x5A, 0x05);  \
__spi_writ_bit16(0x54, 0XD0);  \
__spi_writ_bit16(0x19, 0x05); \
__spi_writ_bit16(0x1A, 0x24);  \
__spi_writ_bit16(0x1B, 0x2A);  \
__spi_writ_bit16(0x1C, 0x0E);  \
__spi_writ_bit16(0x1D, 0x10);  \
__spi_writ_bit16(0x1E, 0x17);  \
__spi_writ_bit16(0x1F, 0XD5);  \
__spi_writ_bit16(0x20, 0XB0);  \
__spi_writ_bit16(0x21, 0x4F);  \
__spi_writ_bit16(0x22, 0x6F); \
__spi_writ_bit16(0x23, 0x06);  \
__spi_writ_bit16(0x24, 0x0E);  \
__spi_writ_bit16(0x25, 0x0F);  \
__spi_writ_bit16(0x26, 0x28);  \
__spi_writ_bit16(0x27, 0x22);  \
__spi_writ_bit16(0x28, 0x08);  \
__spi_writ_bit16(0x29, 0x04);  \
__spi_writ_bit16(0x2A, 0x1F);  \
__spi_writ_bit16(0x2B, 0x22);  \
__spi_writ_bit16(0x2C, 0x0F); \
__spi_writ_bit16(0x2D, 0x11);  \
__spi_writ_bit16(0x2E, 0x06); \
__spi_writ_bit16(0x2F, 0x14);  \
__spi_writ_bit16(0x30, 0x10); \
__spi_writ_bit16(0x31, 0x0D);  \
__spi_writ_bit16(0x32, 0x28); \
__spi_writ_bit16(0x33, 0x23);  \
__spi_writ_bit16(0x34, 0x06); \
 } while (0)
  #endif //auo-8 end
#else

#define __lcd_special_on()   \
 do { \
	 printf("\n EJ027NA-3.0lcd ... \n"); \
	 __spi_writ_bit16(0x05, 0x1E); \
	 __spi_writ_bit16(0x05, 0x5C); \
	 __spi_writ_bit16(0x02, 0x14); \
	 __spi_writ_bit16(0x03, 0x40); \
	 __spi_writ_bit16(0x04, 0x2b); /*0x2b*/\
	 __spi_writ_bit16(0x06, 0x1B); \
	 __spi_writ_bit16(0x07, 0x28); \
	 __spi_writ_bit16(0x0C, 0x06); \
	 __spi_writ_bit16(0x0D, 0x40); \
	 __spi_writ_bit16(0x0E, 0x40); \
	 __spi_writ_bit16(0x0F, 0x40); \
	 __spi_writ_bit16(0x10, 0x40); \
	 __spi_writ_bit16(0x11, 0x40); \
	 __spi_writ_bit16(0x2F, 0x40); \
	 __spi_writ_bit16(0x5A, 0x02); \
 \
	 __spi_writ_bit16(0x30, 0x07); \
	 __spi_writ_bit16(0x31, 0x57); \
	 __spi_writ_bit16(0x32, 0x53); \
	 __spi_writ_bit16(0x33, 0x77); \
	 __spi_writ_bit16(0x34, 0XB8); \
	 __spi_writ_bit16(0x35, 0xBD); \
	 __spi_writ_bit16(0x36, 0XB8); \
	 __spi_writ_bit16(0x37, 0XE7); \
	 __spi_writ_bit16(0x38, 0x04); \
	 __spi_writ_bit16(0x39, 0xFF); \
 \
	 __spi_writ_bit16(0x40, 0x0B); \
	 __spi_writ_bit16(0x41, 0xB8); \
	 __spi_writ_bit16(0x42, 0xAB); \
	 __spi_writ_bit16(0x43, 0XB9); \
	 __spi_writ_bit16(0x44, 0x6A); \
	 __spi_writ_bit16(0x45, 0x56); \
	 __spi_writ_bit16(0x46, 0x61); \
	 __spi_writ_bit16(0x47, 0x08); \
	 __spi_writ_bit16(0x48, 0x0F); \
	 __spi_writ_bit16(0x49, 0x0F); \
 \
	 __spi_writ_bit16(0x2B, 0x01); \
 } while (0)


#endif


#define __lcd_special_off() \
  do { \
    ; \
  } while (0)
#endif


#if defined(CONFIG_JZ4760_LCD_RG_V10)
// #if defined(CONFIG_JZ4760_LEPUS) || defined(CONFIG_JZ4760B_LEPUS)/* board pavo */
	#define SPEN		(32*4+0)       /*LCD_CS*/
	#define SPCK		(32*3+11)       /*LCD_SCL*/
	#define SPDA		(32*4+2)       /*LCD_SDA*/
	#define LCD_RET 	(32*4+4)
// #else
// #error "driver/video/Jzlcd.h, please define SPI pins on your board."
// #endif

#define __spi_writ_bit16(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
\
	value=((reg<<8)|(val&0xFF));	\
\
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	for(no=0;no<16;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x8000)==0x8000)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)


#define __spi_send_value(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
	unsigned char cmd_dat=0; \
	cmd_dat =reg; \
	value = val; \
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	if(cmd_dat)\
		value |= 1<<8;     \
	else \
		value &= ~(1<<8);  \
	for(no=0;no<9;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x100)==0x100)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)

#define spi_send_cmd(cmd)     __spi_send_value(0,cmd)
#define spi_send_data(data)   __spi_send_value(1,data)

#define __lcd_special_pin_init() \
 do { \
	__gpio_as_output(SPEN); /* use SPDA */	\
	__gpio_as_output(SPCK); /* use SPCK */	\
	__gpio_as_output(SPDA); /* use SPDA */	\
	__gpio_as_output(LCD_RET);              \
	udelay(50);	                            \
	__gpio_clear_pin(LCD_RET);              \
	mdelay(100);				    \
	__gpio_set_pin(LCD_RET);		\
 }while (0)

#define __lcd_special_on()   \
 do { \
 	/*printk("\n RG V10 LCD ... \n");*/\
	 __spi_writ_bit16(0x05,0x34);     \
	 mdelay(5); \
	 __spi_writ_bit16(0x05,0x74); \
	 __spi_writ_bit16(0x05,0x75); \
	 mdelay(10); \
 } while (0)

#define __lcd_special_off() \
  do { 									\
    /*__gpio_as_output(GPIO_LCD_PWM);		*/\
	/*__gpio_clear_pin(GPIO_LCD_PWM);		*/\
  } while (0)

#endif /* CONFIG_JZ4760_LCD_RG_V10 */

#if defined(CONFIG_JZ4760_LCD_RG_V21)
// #if defined(CONFIG_JZ4760_LEPUS) || defined(CONFIG_JZ4760B_LEPUS)/* board pavo */
	#define SPEN		(32*4+0)       /*LCD_CS*/
	#define SPCK		(32*3+11)       /*LCD_SCL*/
	#define SPDA		(32*4+2)       /*LCD_SDA*/
	#define LCD_RET 	(32*4+4)
// #else
// #error "driver/video/Jzlcd.h, please define SPI pins on your board."
// #endif

#define __spi_writ_bit16(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
\
	value=((reg<<8)|(val&0xFF));	\
\
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(10); \
	for(no=0;no<16;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x8000)==0x8000)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(10); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(10); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(10); \
} while (0)


#define __spi_send_value(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
	unsigned char cmd_dat=0; \
	cmd_dat =reg; \
	value = val; \
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	if(cmd_dat)\
		value |= 1<<8;     \
	else \
		value &= ~(1<<8);  \
	for(no=0;no<9;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x100)==0x100)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
	 } \
	__gpio_set_pin(SPEN); \
} while (0)

#define spi_send_cmd(cmd)     __spi_send_value(0,cmd)
#define spi_send_data(data)   __spi_send_value(1,data)

#define __lcd_special_pin_init() \
 do { \
	__gpio_as_output(SPEN); /* use SPDA */	\
	__gpio_as_output(SPCK); /* use SPCK */	\
	__gpio_as_output(SPDA); /* use SPDA */	\
	__gpio_as_output(LCD_RET);              \
	udelay(50);	                            \
	__gpio_clear_pin(LCD_RET);              \
	mdelay(100);				    \
	__gpio_set_pin(LCD_RET);		\
 }while (0)

#define __lcd_special_on()   \
do { \
	/*printk("\n EJ027NA-3.0lcd ... \n");*/\
	__spi_writ_bit16(0x05, 0x1E); \
	__spi_writ_bit16(0x05, 0x5C); \
	__spi_writ_bit16(0x02, 0x14); \
	__spi_writ_bit16(0x03, 0x40); \
	__spi_writ_bit16(0x04, 0x0b); /*0x2b*/\
	__spi_writ_bit16(0x06, 0x1B); \
	__spi_writ_bit16(0x07, 0x28); \
	__spi_writ_bit16(0x0C, 0x06); \
	__spi_writ_bit16(0x0D, 0x40); \
	__spi_writ_bit16(0x0E, 0x40); \
	__spi_writ_bit16(0x0F, 0x40); \
	__spi_writ_bit16(0x10, 0x40); \
	__spi_writ_bit16(0x11, 0x40); \
	__spi_writ_bit16(0x2F, 0x40); \
	__spi_writ_bit16(0x5A, 0x02); \
\
	__spi_writ_bit16(0x30, 0x07); \
	__spi_writ_bit16(0x31, 0x57); \
	__spi_writ_bit16(0x32, 0x53); \
	__spi_writ_bit16(0x33, 0x77); \
	__spi_writ_bit16(0x34, 0XB8); \
	__spi_writ_bit16(0x35, 0xBD); \
	__spi_writ_bit16(0x36, 0XB8); \
	__spi_writ_bit16(0x37, 0XE7); \
	__spi_writ_bit16(0x38, 0x04); \
	__spi_writ_bit16(0x39, 0xFF); \
\
	__spi_writ_bit16(0x40, 0x0B); \
	__spi_writ_bit16(0x41, 0xB8); \
	__spi_writ_bit16(0x42, 0xAB); \
	__spi_writ_bit16(0x43, 0XB9); \
	__spi_writ_bit16(0x44, 0x6A); \
	__spi_writ_bit16(0x45, 0x56); \
	__spi_writ_bit16(0x46, 0x61); \
	__spi_writ_bit16(0x47, 0x08); \
	__spi_writ_bit16(0x48, 0x0F); \
	__spi_writ_bit16(0x49, 0x0F); \
\
	__spi_writ_bit16(0x2B, 0x01); \
} while (0)

 #define __lcd_special_off() \
  do { 									\
    __gpio_as_output(GPIO_LCD_PWM);		\
	__gpio_clear_pin(GPIO_LCD_PWM);		\
  } while (0)
#endif /* CONFIG_JZ4760_LCD_RG_V21 */

#if defined(CONFIG_JZ4760_LCD_RG_V30)
// #if defined(CONFIG_JZ4760_LEPUS) || defined(CONFIG_JZ4760B_LEPUS)/* board pavo */
	#define SPEN		(32*4+0)       /*LCD_CS*/
	#define SPCK		(32*3+11)       /*LCD_SCL*/
	#define SPDA		(32*4+2)       /*LCD_SDA*/
	#define LCD_RET 	(32*4+4)
// #else
// #error "driver/video/Jzlcd.h, please define SPI pins on your board."
// #endif

#define __spi_writ_bit16(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
\
	value=((reg<<8)|(val&0xFF));	\
\
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	for(no=0;no<16;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x8000)==0x8000)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)


#define __spi_send_value(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
	unsigned char cmd_dat=0; \
	cmd_dat =reg; \
	value = val; \
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	if(cmd_dat)\
		value |= 1<<8;     \
	else \
		value &= ~(1<<8);  \
	for(no=0;no<9;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x100)==0x100)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)

#define spi_send_cmd(cmd)     __spi_send_value(0,cmd)
#define spi_send_data(data)   __spi_send_value(1,data)

#define __lcd_special_pin_init() \
 do { \
	__gpio_as_output(SPEN); /* use SPDA */	\
	__gpio_as_output(SPCK); /* use SPCK */	\
	__gpio_as_output(SPDA); /* use SPDA */	\
	__gpio_as_output(LCD_RET);              \
	udelay(50);	                            \
	__gpio_clear_pin(LCD_RET);              \
	mdelay(100);				    \
	__gpio_set_pin(LCD_RET);		\
 }while (0)

#define __lcd_special_on()   \
 do { \
 	/*printk("\n auo-ILI8965B ... \n");*/\
__spi_writ_bit16(0x70, 0x06);  \
__spi_writ_bit16(0x05, 0x81);   \
mdelay(48); \
__spi_writ_bit16(0x70, 0x07);   \
__spi_writ_bit16(0x06, 0x08);    \
__spi_writ_bit16(0x01, 0x59);   \
__spi_writ_bit16(0x03, 0x10);  \
__spi_writ_bit16(0x04, 0x10);   \
__spi_writ_bit16(0x08, 0x20);   \
__spi_writ_bit16(0x0A, 0x84);   \
__spi_writ_bit16(0x5A, 0x05);  \
__spi_writ_bit16(0x54, 0XD0);  \
__spi_writ_bit16(0x19, 0x05); \
__spi_writ_bit16(0x1A, 0x24);  \
__spi_writ_bit16(0x1B, 0x2A);  \
__spi_writ_bit16(0x1C, 0x0E);  \
__spi_writ_bit16(0x1D, 0x10);  \
__spi_writ_bit16(0x1E, 0x17);  \
__spi_writ_bit16(0x1F, 0XD5);  \
__spi_writ_bit16(0x20, 0XB0);  \
__spi_writ_bit16(0x21, 0x4F);  \
__spi_writ_bit16(0x22, 0x6F); \
__spi_writ_bit16(0x23, 0x06);  \
__spi_writ_bit16(0x24, 0x0E);  \
__spi_writ_bit16(0x25, 0x0F);  \
__spi_writ_bit16(0x26, 0x28);  \
__spi_writ_bit16(0x27, 0x22);  \
__spi_writ_bit16(0x28, 0x08);  \
__spi_writ_bit16(0x29, 0x04);  \
__spi_writ_bit16(0x2A, 0x1F);  \
__spi_writ_bit16(0x2B, 0x22);  \
__spi_writ_bit16(0x2C, 0x0F); \
__spi_writ_bit16(0x2D, 0x11);  \
__spi_writ_bit16(0x2E, 0x06); \
__spi_writ_bit16(0x2F, 0x14);  \
__spi_writ_bit16(0x30, 0x10); \
__spi_writ_bit16(0x31, 0x0D);  \
__spi_writ_bit16(0x32, 0x28); \
__spi_writ_bit16(0x33, 0x23);  \
__spi_writ_bit16(0x34, 0x06); \
 } while (0)


 #define __lcd_special_off() \
  do { 									\
    __gpio_as_output(GPIO_LCD_PWM);		\
	__gpio_clear_pin(GPIO_LCD_PWM);		\
  } while (0)


#endif /* CONFIG_JZ4760_LCD_RG_V30 */

#if defined(CONFIG_JZ4760_LCD_RG_IPS)
// #if defined(CONFIG_JZ4760_LEPUS) || defined(CONFIG_JZ4760B_LEPUS)/* board pavo */
	#define SPEN		(32*4+0)       /*LCD_CS*/
	#define SPCK		(32*3+11)       /*LCD_SCL*/
	#define SPDA		(32*4+2)       /*LCD_SDA*/
	#define LCD_RET 	(32*4+4)
// #else
// #error "driver/video/Jzlcd.h, please define SPI pins on your board."
// #endif

#define __spi_writ_bit16(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
\
	value=((reg<<8)|(val&0xFF));	\
\
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	for(no=0;no<16;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x8000)==0x8000)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)


#define __spi_send_value(reg, val) \
do { \
	unsigned char no; \
	unsigned short value; \
	unsigned char cmd_dat=0; \
	cmd_dat =reg; \
	value = val; \
	__gpio_set_pin(SPEN); \
	__gpio_clear_pin(SPCK); \
	__gpio_clear_pin(SPDA); \
	__gpio_clear_pin(SPEN); \
	udelay(50); \
	if(cmd_dat)\
		value |= 1<<8;     \
	else \
		value &= ~(1<<8);  \
	for(no=0;no<9;no++)\
	{ \
		__gpio_clear_pin(SPCK); \
		if((value&0x100)==0x100)\
			__gpio_set_pin(SPDA); \
		else\
			__gpio_clear_pin(SPDA); \
		udelay(50); \
		__gpio_set_pin(SPCK); \
		value <<= 1; \
		udelay(50); \
	 } \
	__gpio_set_pin(SPEN); \
	udelay(50); \
} while (0)

#define spi_send_cmd(cmd)     __spi_send_value(0,cmd)
#define spi_send_data(data)   __spi_send_value(1,data)

#define __lcd_special_pin_init() \
 do { \
	__gpio_as_output(SPEN); /* use SPDA */	\
	__gpio_as_output(SPCK); /* use SPCK */	\
	__gpio_as_output(SPDA); /* use SPDA */	\
	__gpio_as_output(LCD_RET);              \
	udelay(50);	                            \
	__gpio_clear_pin(LCD_RET);              \
	mdelay(100);				    \
	__gpio_set_pin(LCD_RET);		\
 }while (0)


#define __lcd_special_on() \
do{ \
__spi_writ_bit16(0x02,0x7f); \
__spi_writ_bit16(0x03,0x0A); \
__spi_writ_bit16(0x04,0x80); \
__spi_writ_bit16(0x06,0x90); \
__spi_writ_bit16(0x08,0x28); \
__spi_writ_bit16(0x09,0x20); \
__spi_writ_bit16(0x0a,0x20); \
__spi_writ_bit16(0x0c,0x10); \
__spi_writ_bit16(0x0d,0x10); \
__spi_writ_bit16(0x0e,0x10); \
__spi_writ_bit16(0x10,0x7F); \
__spi_writ_bit16(0x11,0x3F); \
}while(0)
#endif /* CONFIG_JZ4760_LCD_RG_IPS */

#ifndef __lcd_special_pin_init
#define __lcd_special_pin_init()
#endif
#ifndef __lcd_special_on
#error
#define __lcd_special_on()
#endif
#ifndef __lcd_special_off
#define __lcd_special_off()
#endif

/*
 * Platform specific definition
 */
#if defined(CONFIG_APUS) /* board apus */
/*======================================================================
 * LCD backlight
 */
 #error "ddddd"
#define LCD_PWM_FULL 101
/* 100 level: 0,1,...,100 */
#define __lcd_set_backlight_level(n)	\
do {					\
	__gpio_as_output(GPIO_LCD_PWM);	\
	__gpio_set_pin(GPIO_LCD_PWM);	\
} while (0)

#define __lcd_close_backlight()		\
do {					\
	__gpio_as_output(GPIO_LCD_PWM);	\
	__gpio_clear_pin(GPIO_LCD_PWM);	\
} while (0)

#define __lcd_display_pin_init() \
do { \
	__gpio_as_output(GPIO_LCD_VCC_EN_N);	 \
	__lcd_special_pin_init();		 \
} while (0)
#define __lcd_display_on() \
do { \
	__gpio_clear_pin(GPIO_LCD_VCC_EN_N);	\
	__lcd_special_on();			\
	__lcd_set_backlight_level(80);		\
} while (0)

#define __lcd_display_off() \
do { \
	__lcd_close_backlight();	   \
	__lcd_special_off();	 \
} while (0)

/*=================================================================
my board */
#elif defined(CONFIG_JZ4760) || defined(CONFIG_JZ4760B)

#define LCD_PWM_FREQ  15000  /* pwm freq */
#define LCD_PWM_CHN 1   /* pwm channel */

//don't change
#define LCD_PWM_FULL 101//150//256
#define LCD_DEFAULT_BACKLIGHT		80
#define LCD_MAX_BACKLIGHT		100
#define LCD_MIN_BACKLIGHT		1


#define __lcd_init_backlight(n)					\
do {    							\
	__lcd_set_backlight_level(n);				\
} while (0)

/* 100 level: 0,1,...,100 */

#if 0 //allen test
#define __lcd_set_backlight_level(n)				\
do {								\
	__gpio_as_output(GPIO_LCD_PWM); 			\
	__gpio_set_pin(GPIO_LCD_PWM); 		\
} while (0)

#define __lcd_close_backlight()					\
do {								\
	__gpio_as_output(GPIO_LCD_PWM); 			\
	__gpio_set_pin(GPIO_LCD_PWM); 		\
} while (0)

#else
#define __lcd_set_backlight_level(n)				\
do {								\
	__gpio_as_pwm(LCD_PWM_CHN);                               \
	__tcu_disable_pwm_output(LCD_PWM_CHN);			\
	__tcu_stop_counter(LCD_PWM_CHN);			\
	__tcu_init_pwm_output_high(LCD_PWM_CHN);		\
	__tcu_set_pwm_output_shutdown_abrupt(LCD_PWM_CHN);	\
	__tcu_select_clk_div1(LCD_PWM_CHN);			\
	__tcu_mask_full_match_irq(LCD_PWM_CHN);			\
	__tcu_mask_half_match_irq(LCD_PWM_CHN);			\
	__tcu_clear_counter_to_zero(LCD_PWM_CHN);		\
	__tcu_set_full_data(LCD_PWM_CHN, JZ_EXTAL / LCD_PWM_FREQ);	\
	__tcu_set_half_data(LCD_PWM_CHN, JZ_EXTAL / LCD_PWM_FREQ * n / LCD_PWM_FULL); \
	__tcu_enable_pwm_output(LCD_PWM_CHN);			\
	__tcu_select_extalclk(LCD_PWM_CHN);			\
	__tcu_start_counter(LCD_PWM_CHN);			\
} while (0)

#define __lcd_close_backlight()					\
do {								\
	__gpio_as_output(GPIO_LCD_PWM); 			\
	__gpio_clear_pin(GPIO_LCD_PWM); 		\
} while (0)
#endif

#define __lcd_display_pin_init() \
do { \
	__gpio_as_output(GPIO_LCD_PWM);	 \
	__lcd_special_pin_init();	   \
} while (0)
#define __lcd_display_on() \
do { \
	__lcd_special_on();			\
} while (0)

#define __lcd_display_off() \
do { \
	__lcd_close_backlight();	   \
	__lcd_special_off();	 \
} while (0)

/*=================================================================
other board */
#else // apus
#error "ddddd"

do { \
} while (0)
#define __lcd_display_on() \
do { \
	__lcd_special_on();			\
	__lcd_set_backlight_level(80);		\
} while (0)

#define __lcd_display_off() \
do { \
	__lcd_close_backlight();	   \
	__lcd_special_off();	 \
} while (0)
#endif /* APUS */


/*****************************************************************************
 * LCD display pin dummy macros
 *****************************************************************************/

#ifndef __lcd_display_pin_init
#define __lcd_display_pin_init()
#endif
#ifndef __lcd_slcd_special_on
#define __lcd_slcd_special_on()
#endif
#ifndef __lcd_display_on
#define __lcd_display_on()
#endif
#ifndef __lcd_display_off
#define __lcd_display_off()
#endif
#ifndef __lcd_set_backlight_level
#define __lcd_set_backlight_level(n)
#endif

#endif /* __JZ4760_LCD_H__ */
