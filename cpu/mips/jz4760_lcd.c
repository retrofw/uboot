/*
 * JzRISC lcd controller
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/************************************************************************/
/* ** HEADER FILES							*/
/************************************************************************/

/*
 * Fallowing macro may be used:
 *  CONFIG_LCD                        : LCD support
 *  LCD_BPP                           : Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8
 *  CFG_WHITE_ON_BLACK
 *  CONFIG_LCD_LOGO                   : show logo
 *  CFG_LCD_LOGOONLY_NOINFO           : not display info on lcd screen, only logo
 * -----------------------------------------------------------------------
 * bugs:
 * if BMP_LOGO_HEIGHT > (lcd screen height - 2*VIDEO_FONT_HEIGHT),
 * must not print info onto screen,
 * it means should define CFG_LCD_LOGOONLY_NOINFO.
 */


#include <config.h>
#include <common.h>
#include <devices.h>
#include <lcd.h>

#include <asm/io.h>               /* virt_to_phys() */

#if defined(CONFIG_JZ4750) || defined(CONFIG_JZ4750D) || defined(CONFIG_JZ4750L) || defined(CONFIG_JZ4760B) || defined(CONFIG_JZ4760)
#if defined(CONFIG_LCD) && !defined(CONFIG_SLCD)
#if defined(CONFIG_JZ4750)
#include <asm/jz4750.h>
#endif

#if defined(CONFIG_JZ4760B)
#include <asm/jz4760b.h>
#endif
#if defined(CONFIG_JZ4760)
#include <asm/jz4760.h>
#endif

#include <config.h>

#include "jz4760_lcd.h"

#define BACKLIGHT_LEVEL    30//20
#define BATTERY_STEPS  3

#include "bootlogo/RetroFW2.h"
#include "bootlogo/battery_0.h"
#include "bootlogo/battery_1.h"
#include "bootlogo/battery_2.h"

#ifdef CFG_SUPPORT_RECOVERY_KEY
	#include "bootlogo/sd.h"
	extern int is_recovery_keys_pressed(void);
#endif

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#define dprintf(x...)	printf(x)
#else
#define dprintf(x...)
#endif

#define PRINTF_DEBUG dprintf("%s %d\n",__FILE__,__LINE__)

static struct jz4750_lcd_dma_desc *dma_desc_base = NULL;
static struct jz4750_lcd_dma_desc *dma0_desc_palette = NULL, *dma0_desc0 = NULL, *dma0_desc1 = NULL, *dma1_desc0 = NULL, *dma1_desc1 = NULL;
#if defined(CONFIG_FB_JZ4750_SLCD)
static unsigned char *lcd_cmdbuf;
#endif

#define DMA_DESC_NUM 		6
static struct jz4750_lcd_dma_desc *dma0_desc_cmd0 = NULL, *dma0_desc_cmd = NULL;
static unsigned char *lcd_palette;
static unsigned char *lcd_frame0;
static unsigned char *lcd_frame1;

static void ctrl_enable(void);
static void ctrl_disable(void);

struct jz4750lcd_info jzfb = {
#if defined(CONFIG_JZ4750_LCD_SAMSUNG_LTP400WQF02)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_GENERIC_TFT | /* General TFT panel */
		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP,	/* Vsync polarity: leading edge is falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		480, 272, 60, 41, 10, 2, 2, 2, 2,
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 480, 272}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 480, 272}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_LCD_AUO_A043FL01V2)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_GENERIC_TFT | /* General TFT panel */
		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP,	/* Vsync polarity: leading edge is falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		480, 272, 60, 41, 10, 8, 4, 4, 2,
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_F1EN | /* enable Foreground1 */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x0000ff, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_LCD_TRULY_TFT_GG1P0319LTSW_W)
	.panel = {
//		 .cfg = LCD_CFG_LCDPIN_SLCD | LCD_CFG_RECOVER | /* Underrun recover*/
		 .cfg = LCD_CFG_LCDPIN_SLCD | /* Underrun recover*/
		 LCD_CFG_NEWDES | /* 8words descriptor */
		 LCD_CFG_MODE_SLCD, /* TFT Smart LCD panel */
		 .slcd_cfg = SLCD_CFG_DWIDTH_16BIT | SLCD_CFG_CWIDTH_16BIT | SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING | SLCD_CFG_TYPE_PARALLEL,
		 .ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		 240, 320, 60, 0, 0, 0, 0, 0, 0,
	 },
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
		 LCD_OSDC_F1EN | /* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 240, 320}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 240, 320}, /* bpp, x, y, w, h */
	 },

#elif defined(CONFIG_JZ4750_LCD_FOXCONN_PT035TN01)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_GENERIC_TFT | /* General TFT panel */
//		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_MODE_TFT_24BIT | 	/* output 24bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP |	/* Vsync polarity: leading edge is falling edge */
		LCD_CFG_PCP,	/* Pix-CLK polarity: data translations at falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		320, 240, 80, 1, 1, 10, 50, 10, 13
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_F1EN |	/* enable Foreground1 */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_LCD_INNOLUX_PT035TN01_SERIAL)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */
		LCD_CFG_NEWDES | /* 8words descriptor */
		LCD_CFG_MODE_SERIAL_TFT | /* Serial TFT panel */
		LCD_CFG_MODE_TFT_18BIT | 	/* output 18bpp */
		LCD_CFG_HSP | 	/* Hsync polarity: active low */
		LCD_CFG_VSP |	/* Vsync polarity: leading edge is falling edge */
		LCD_CFG_PCP,	/* Pix-CLK polarity: data translations at falling edge */
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		320, 240, 60, 1, 1, 10, 50, 10, 13
	},
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 320, 240}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4750_SLCD_KGM701A3_TFT_SPFD5420A)
	.panel = {
//		 .cfg = LCD_CFG_LCDPIN_SLCD | LCD_CFG_RECOVER | /* Underrun recover*/
		 .cfg = LCD_CFG_LCDPIN_SLCD | /* Underrun recover*/
//		 LCD_CFG_DITHER | /* dither */
		 LCD_CFG_NEWDES | /* 8words descriptor */
		 LCD_CFG_MODE_SLCD, /* TFT Smart LCD panel */
		 .slcd_cfg = SLCD_CFG_DWIDTH_18BIT | SLCD_CFG_CWIDTH_18BIT | SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING | SLCD_CFG_TYPE_PARALLEL,
		 .ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		 400, 240, 60, 0, 0, 0, 0, 0, 0,
	 },
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_ALPHAMD | /* alpha blending mode */
//		 LCD_OSDC_F1EN | /* enable Foreground1 */
		 LCD_OSDC_F0EN,	/* enable Foreground0 */
		 .osd_ctrl = 0,		/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0,	/* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {32, 0, 0, 400, 240}, /* bpp, x, y, w, h */
		 .fg1 = {32, 0, 0, 400, 240}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4760_LCD_SNK)
	#if 0 //auo and ILI8965
		.panel = {
			.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */
				 LCD_CFG_MODE_SERIAL_TFT | /* 8bit serial TFT */
				 LCD_CFG_MODE_TFT_16BIT | /* output 18bpp */
				 LCD_CFG_PCP | LCD_CFG_NEWDES,

			.slcd_cfg = 0,
			.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	  /* 16words burst, enable out FIFO underrun irq */
			//320,480, 120, 20, 1, 48, 40, 18, 27,//auo
			320, 480, 60, 20, 1, 48, 40, 10,42,//ILI8965
		},

		.osd = {
			 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
			 //LCD_OSDC_ALPHAEN | /* enable alpha */
			 //LCD_OSDC_F1EN |  /* enable Foreground0 */
			 LCD_OSDC_F0EN, /* enable Foreground0 */
			 .osd_ctrl = 0,	  /* disable ipu,  */
			 .rgb_ctrl = 0x30,//0x03,
			 .bgcolor = 0x000000, /* set background color Black */
			 .colorkey0 = 0x80000000, /* disable colorkey */
			 .colorkey1 = 0x80000000, /* disable colorkey */
			 .alpha = 0xA0, /* alpha value */
			 .ipu_restart = 0x80001000, /* ipu restart */
			 .fg_change = FG_CHANGE_ALL, /* change all initially */
			 .fg0 = {16, 0, 0, 320, 480}, /* bpp, x, y, w, h */
			 .fg1 = {16, 0, 0, 320, 480}, /* bpp, x, y, w, h */
		 },
	#else
		 .panel = {
			.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER | /* Underrun recover */
				 LCD_CFG_MODE_SERIAL_TFT | /* 8bit serial TFT */
				 LCD_CFG_MODE_TFT_16BIT | /* output 18bpp */
				 LCD_CFG_PCP | LCD_CFG_NEWDES,

			.slcd_cfg = 0,
			.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	  /* 16words burst, enable out FIFO underrun irq */
			320,480, 60, 20, 1, 32,40, 17, 27,
		},

		.osd = {
			 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
			 //LCD_OSDC_ALPHAEN | /* enable alpha */
			 //LCD_OSDC_F1EN |  /* enable Foreground0 */
			 LCD_OSDC_F0EN, /* enable Foreground0 */
			 .osd_ctrl = 0,	  /* disable ipu,  */
			 .rgb_ctrl = 0x0,
			 .bgcolor = 0x000000, /* set background color Black */
			 .colorkey0 = 0x80000000, /* disable colorkey */
			 .colorkey1 = 0x80000000, /* disable colorkey */
			 .alpha = 0xA0, /* alpha value */
			 .ipu_restart = 0x80001000, /* ipu restart */
			 .fg_change = FG_CHANGE_ALL, /* change all initially */
			 .fg0 = {16, 0, 0, 320, 480}, /* bpp, x, y, w, h */
			 .fg1 = {16, 0, 0, 320, 480}, /* bpp, x, y, w, h */
		 },
	#endif

#elif defined(CONFIG_JZ4760_SLCD_SNK)
	.panel = {
		 .cfg = LCD_CFG_LCDPIN_SLCD | /* Underrun recover*/
		 LCD_CFG_NEWDES | /* 8words descriptor */
		 LCD_CFG_MODE_SLCD, /* TFT Smart LCD panel */
		 .slcd_cfg = SLCD_CFG_DWIDTH_8_x1| SLCD_CFG_CWIDTH_8BIT|SLCD_CFG_CS_ACTIVE_LOW|\
		 SLCD_CFG_RS_CMD_LOW |SLCD_CFG_CLK_ACTIVE_FALLING| SLCD_CFG_TYPE_PARALLEL,
		 .ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,	/* 16words burst, enable out FIFO underrun irq */
		 240, 320, 60, 0, 0, 0, 0, 0, 0,
	 },
	.osd = {
		 .osd_cfg = LCD_OSDC_OSDEN | /* Use OSD mode */
			//LCD_OSDC_ALPHAMD| /*for 32bpp*/
//		 LCD_OSDC_ALPHAEN | /* enable alpha */
//		 LCD_OSDC_F1EN | /* enable Foreground0 */
		 LCD_OSDC_F0EN, /* enable Foreground0 */
		 .osd_ctrl = 0, 	/* disable ipu,  */
		 .rgb_ctrl = 0,
		 .bgcolor = 0x000000, /* set background color Black */
		 .colorkey0 = 0, /* disable colorkey */
		 .colorkey1 = 0, /* disable colorkey */
		 .alpha = 0xA0, /* alpha value */
		 .ipu_restart = 0x80001000, /* ipu restart */
		 .fg_change = FG_CHANGE_ALL, /* change all initially */
		 .fg0 = {16, 0, 0, 240, 320}, /* bpp, x, y, w, h */
		 .fg1 = {16, 0, 0, 240, 320}, /* bpp, x, y, w, h */
	 },
#elif defined(CONFIG_JZ4760_LCD_RG_V10)
    .panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER |	// Underrun recover
			   LCD_CFG_MODE_SERIAL_TFT |				// General TFT panel
			   LCD_CFG_MODE_TFT_16BIT |
			   LCD_CFG_PCP |
			   LCD_CFG_NEWDES,							// 8words descriptor
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,		// 16words burst, enable out FIFO underrun irq
		320, 480, 60, 20, 1, 48, 40, 18,27,
	},
	.osd = {
		.osd_cfg = LCD_OSDC_OSDEN |			// Use OSD mode
					 // LCD_OSDC_ALPHAEN |	// enable alpha
					 LCD_OSDC_F0EN,		// enable Foreground0
					 // LCD_OSDC_F1EN,			// enable Foreground1
		.osd_ctrl = 0,
		.rgb_ctrl = LCD_RGBC_EVEN_GBR << LCD_RGBC_EVENRGB_BIT,
		.bgcolor = 0x000000,				// set background color Black
		.colorkey0 = 0x80000000,			// disable colorkey
		.colorkey1 = 0x80000000,			// disable colorkey
		.alpha = 0xA0,						// alpha value
		.ipu_restart = 0x80001000,			// ipu restart
		.fg_change = FG_CHANGE_ALL,			// change all initially
		.fg0 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
		.fg1 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
	 },

#elif defined(CONFIG_JZ4760_LCD_RG_V21)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER |	// Underrun recover
				 LCD_CFG_MODE_SERIAL_TFT |				// General TFT panel
				 LCD_CFG_MODE_TFT_16BIT |
				 LCD_CFG_PCP,								// Vsync polarity: leading edge is falling edge
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,		// 16words burst, enable out FIFO underrun irq
	/* width,height,freq,hsync,vsync,elw,blw,efw,bfw */
		320, 480, 60, 20, 1, 32, 40, 17, 27,	//INNOLUX
	},
	.osd = {
		.osd_cfg = LCD_OSDC_OSDEN |			// Use OSD mode
					 // LCD_OSDC_ALPHAEN |	// enable alpha
					 LCD_OSDC_F0EN,		// enable Foreground0
					 // LCD_OSDC_F1EN,			// enable Foreground1
		.osd_ctrl = 0,
		.rgb_ctrl = LCD_RGBC_EVEN_GBR << LCD_RGBC_EVENRGB_BIT,
		.bgcolor = 0x000000,				// set background color Black
		.colorkey0 = 0x80000000,			// disable colorkey
		.colorkey1 = 0x80000000,			// disable colorkey
		.alpha = 0xA0,						// alpha value
		.ipu_restart = 0x80001000,			// ipu restart
		.fg_change = FG_CHANGE_ALL,			// change all initially
		.fg0 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
		.fg1 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
	},

#elif defined(CONFIG_JZ4760_LCD_RG_V30)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER |	// Underrun recover
				 LCD_CFG_MODE_SERIAL_TFT |				// General TFT panel
				 LCD_CFG_MODE_TFT_16BIT |
				 LCD_CFG_PCP |
				 LCD_CFG_NEWDES,							// 8words descriptor
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,		// 16words burst, enable out FIFO underrun irq
	/* width,height,freq,hsync,vsync,elw,blw,efw,bfw */
		320, 480, 59, 20, 1, 48, 40, 10, 42,	//ILI8965
	},
	.osd = {
		.osd_cfg = LCD_OSDC_OSDEN |			// Use OSD mode
					 // LCD_OSDC_ALPHAEN |	// enable alpha
					 LCD_OSDC_F0EN,		// enable Foreground0
					 // LCD_OSDC_F1EN,			// enable Foreground1
		.osd_ctrl = 0,
		.rgb_ctrl = LCD_RGBC_ODD_GBR << LCD_RGBC_ODDRGB_BIT,
		.bgcolor = 0x000000,				// set background color Black
		.colorkey0 = 0x80000000,			// disable colorkey
		.colorkey1 = 0x80000000,			// disable colorkey
		.alpha = 0xA0,						// alpha value
		.ipu_restart = 0x80001000,			// ipu restart
		.fg_change = FG_CHANGE_ALL,			// change all initially
		.fg0 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
		.fg1 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
	},
#elif defined(CONFIG_JZ4760_LCD_RG_IPS)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER |	// Underrun recover
				 LCD_CFG_NEWDES |							// 8words descriptor
				 LCD_CFG_MODE_SERIAL_TFT |				// LCD_CFG_MODE_SERIAL_TFT
				 LCD_CFG_HSP|
				 LCD_CFG_MODE_TFT_16BIT,
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,		// 16words burst, enable out FIFO underrun irq
	/* width,height,freq,hsync,vsync,elw,blw,efw,bfw */
		320, 480, 60, 28, 1, 25, 100, 16,36,
	},
	.osd = {
		.osd_cfg = LCD_OSDC_OSDEN |			// Use OSD mode
					 // LCD_OSDC_ALPHAEN |	// enable alpha
					 LCD_OSDC_F0EN,		// enable Foreground0
					 // LCD_OSDC_F1EN,			// enable Foreground1
		.osd_ctrl = 0,
		.rgb_ctrl = LCD_RGBC_ODD_GBR << LCD_RGBC_ODDRGB_BIT,
		.bgcolor = 0x000000,				// set background color Black
		.colorkey0 = 0x80000000,			// disable colorkey
		.colorkey1 = 0x80000000,			// disable colorkey
		.alpha = 0xA0,						// alpha value
		.ipu_restart = 0x80001000,			// ipu restart
		.fg_change = FG_CHANGE_ALL,			// change all initially
		.fg0 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
		.fg1 = {16, 0, 0, 320, 480},		// bpp, x, y, w, h
	},
#elif defined(CONFIG_JZ4760_LCD_TM370_LN430_9)
	.panel = {
		.cfg = LCD_CFG_LCDPIN_LCD | LCD_CFG_RECOVER |	// Underrun recover
				 LCD_CFG_NEWDES |
				 LCD_CFG_MODE_GENERIC_TFT |				// General TFT panel
				 LCD_CFG_MODE_TFT_18BIT |					// output 18bpp
				 LCD_CFG_HSP | 							// Hsync polarity: active low
				 LCD_CFG_VSP,
		.slcd_cfg = 0,
		.ctrl = LCD_CTRL_OFUM | LCD_CTRL_BST_16,		// 16words burst, enable out FIFO underrun irq
	/* width,height,freq,hsync,vsync,elw,blw,efw,bfw */
		480, 272, 59, 41, 10, 2, 2, 2, 2,
	},
	.osd = {
		.osd_cfg = LCD_OSDC_OSDEN |			// Use OSD mode
					 // LCD_OSDC_ALPHAEN |	// enable alpha
					 LCD_OSDC_F0EN,		// enable Foreground0
					 // LCD_OSDC_F1EN,			// enable Foreground1
		.osd_ctrl = 0,
		.rgb_ctrl = 0,
		.bgcolor = 0x000000,				// set background color Black
		.colorkey0 = 0x80000000,			// disable colorkey
		.colorkey1 = 0x80000000,			// disable colorkey
		.alpha = 0xA0,						// alpha value
		.ipu_restart = 0x80001000,			// ipu restart
		.fg_change = FG_CHANGE_ALL,			// change all initially
		.fg0 = {16, 0, 0, 480, 272},		// bpp, x, y, w, h
		.fg1 = {16, 0, 0, 480, 272},		// bpp, x, y, w, h
	 },
#else
#error "Select LCD panel first!!!"
#endif
};
struct jz4750lcd_info *jz4750_lcd_info = &jzfb; /* default output to lcd panel */

/************************************************************************/

vidinfo_t panel_info = {
#if defined(CONFIG_JZ4750_LCD_SAMSUNG_LTP400WQF02) || defined(CONFIG_JZ4750_LCD_AUO_A043FL01V2) || defined(CONFIG_JZ4750_LCD_L430) || defined(CONFIG_JZ4760_LCD_TM370_LN430_9)
	480, 272, LCD_BPP,
#elif defined(CONFIG_JZ4750_LCD_TRULY_TFT_GG1P0319LTSW_W) || defined(CONFIG_JZ4760_SLCD_SNK)
	240, 320, LCD_BPP,
#elif defined(CONFIG_JZ4750_LCD_FOXCONN_PT035TN01) || defined(CONFIG_JZ4750_LCD_INNOLUX_PT035TN01_SERIAL)
	320, 240, LCD_BPP,
#elif defined(CONFIG_JZ4750_SLCD_KGM701A3_TFT_SPFD5420A)
	400, 240, LCD_BPP,
#elif defined(CONFIG_JZ4760_LCD_SNK) || defined(CONFIG_JZ4760_LCD_RG_V10) || defined(CONFIG_JZ4760_LCD_RG_V21) || defined(CONFIG_JZ4760_LCD_RG_V30) || defined(CONFIG_JZ4760_LCD_RG_IPS)
	320, 480, LCD_COLOR16,
#else
	#error "Select LCD panel first!!!"
#endif

};


/*----------------------------------------------------------------------*/

int lcd_line_length;

int lcd_color_fg;
int lcd_color_bg;

/*
 * Frame buffer memory information
 */
void *lcd_base;			/* Start of framebuffer memory	*/
void *lcd_console_address;	/* Start of console buffer	*/

short console_col;
short console_row;

/************************************************************************/

void lcd_ctrl_init (void *lcdbase);

void lcd_enable (void);

/************************************************************************/

static int  jz_lcd_init_mem(void *lcdbase, vidinfo_t *vid);
//static void jz_lcd_desc_init(vidinfo_t *vid);
//static int  jz_lcd_hw_init( vidinfo_t *vid );
extern int flush_cache_all(void);

#if LCD_BPP == LCD_COLOR8
void lcd_setcolreg (ushort regno, ushort red, ushort green, ushort blue);
#endif
#if LCD_BPP == LCD_MONOCHROME
void lcd_initcolregs (void);
#endif

/************************************************************************/
static void jz4750fb_change_clock( struct jz4750lcd_info * lcd_info )
{
	unsigned int val = 0;
	unsigned int pclk;
	/* Timing setting */
	__cpm_stop_lcd();

	val = lcd_info->panel.fclk; /* frame clk */

	if ( (lcd_info->panel.cfg & LCD_CFG_MODE_MASK) != LCD_CFG_MODE_SERIAL_TFT) {
		pclk = val * (lcd_info->panel.w + lcd_info->panel.hsw + lcd_info->panel.elw + lcd_info->panel.blw) * (lcd_info->panel.h + lcd_info->panel.vsw + lcd_info->panel.efw + lcd_info->panel.bfw); /* Pixclk */
	}
	else {
		/* serial mode: Hsync period = 3*Width_Pixel */
		pclk = val * (lcd_info->panel.w*3 + lcd_info->panel.hsw + lcd_info->panel.elw + lcd_info->panel.blw) * (lcd_info->panel.h + lcd_info->panel.vsw + lcd_info->panel.efw + lcd_info->panel.bfw); /* Pixclk */
	}

	/********* In TVE mode PCLK = 27MHz ***********/
	if ( lcd_info->panel.cfg & LCD_CFG_TVEN ) { 		/* LCDC output to TVE */
	}
	else { 		/* LCDC output to  LCD panel */
		//pclk = 12000000;
		//pclk = 22000000;
		val = __cpm_get_pllout2() / pclk; /* pclk */
		dprintf("pllout2 = %d plck is %d\n", __cpm_get_pllout2(),pclk);
		val--;
		dprintf("ratio: val = %d\n", val);
		if ( val > 0x7ff ) {
			dprintf("pixel clock divid is too large, set it to 0x7ff\n");
			val = 0x7ff;
		}
		__cpm_set_pixdiv(val);
		dprintf("REG_CPM_LPCDR = 0x%08x\n", REG_CPM_LPCDR);
#if defined(CONFIG_SOC_JZ4750) /* Jz4750D don't use LCLK */
		val = pclk * 3 ;	/* LCDClock > 2.5*Pixclock */
		val =__cpm_get_pllout2() / val;
		if ( val > 0x1f ) {
			dprintf("lcd clock divide is too large, set it to 0x1f\n");
			val = 0x1f;
		}
		__cpm_set_ldiv( val );
#endif
		__cpm_select_pixclk_lcd();
		REG_CPM_CPCCR |= CPM_CPCCR_CE ; /* update divide */

	}

	dprintf("REG_CPM_LPCDR=0x%08x\n", REG_CPM_LPCDR);
	dprintf("REG_CPM_CPCCR=0x%08x\n", REG_CPM_CPCCR);

	__cpm_start_lcd();
	udelay(1000);
	/*
	 * set lcd device clock and lcd pixel clock.
	 * what about TVE mode???
	 *
	 */
}
static void jz4750fb_set_osd_mode( struct jz4750lcd_info * lcd_info )
{
	dprintf("%s, %d\n", __FILE__, __LINE__ );
	lcd_info->osd.osd_ctrl &= ~(LCD_OSDCTRL_OSDBPP_MASK);
	if ( lcd_info->osd.fg1.bpp == 15 )
		lcd_info->osd.osd_ctrl |= LCD_OSDCTRL_OSDBPP_15_16|LCD_OSDCTRL_RGB555;
	else if ( lcd_info->osd.fg1.bpp == 16 )
		lcd_info->osd.osd_ctrl |= LCD_OSDCTRL_OSDBPP_15_16|LCD_OSDCTRL_RGB565;
	else {
		lcd_info->osd.fg1.bpp = 32;
		lcd_info->osd.osd_ctrl |= LCD_OSDCTRL_OSDBPP_18_24;
	}

	REG_LCD_OSDC 	= lcd_info->osd.osd_cfg; /* F0, F1, alpha, */

	REG_LCD_OSDCTRL = lcd_info->osd.osd_ctrl; /* IPUEN, bpp */
	REG_LCD_RGBC  	= lcd_info->osd.rgb_ctrl;
	REG_LCD_BGC  	= lcd_info->osd.bgcolor;
	REG_LCD_KEY0 	= lcd_info->osd.colorkey0;
	REG_LCD_KEY1 	= lcd_info->osd.colorkey1;
	REG_LCD_ALPHA 	= lcd_info->osd.alpha;
	REG_LCD_IPUR 	= lcd_info->osd.ipu_restart;
}
static void jz4750fb_foreground_resize( struct jz4750lcd_info * lcd_info )
{
	int fg0_line_size, fg0_frm_size, fg1_line_size, fg1_frm_size;
	/*
	 * NOTE:
	 * Foreground change sequence:
	 * 	1. Change Position Registers -> LCD_OSDCTL.Change;
	 * 	2. LCD_OSDCTRL.Change -> descripter->Size
	 * Foreground, only one of the following can be change at one time:
	 * 	1. F0 size;
	 *	2. F0 position
	 * 	3. F1 size
	 *	4. F1 position
	 */

	/*
	 * The rules of f0, f1's position:
	 * 	f0.x + f0.w <= panel.w;
	 * 	f0.y + f0.h <= panel.h;
	 *
	 * When output is LCD panel, fg.y and fg.h can be odd number or even number.
	 * When output is TVE, as the TVE has odd frame and even frame,
	 * to simplified operation, fg.y and fg.h should be even number always.
	 *
	 */
	dprintf("the fg0x is %d fg0y is %d fg0w is %d fgoh is %d\n",lcd_info->osd.fg0.x,lcd_info->osd.fg0.y,lcd_info->osd.fg0.w,lcd_info->osd.fg0.h);
	dprintf("the fg1x is %d fg1y is %d fg1w is %d fg1h is %d\n",lcd_info->osd.fg1.x,lcd_info->osd.fg1.y,lcd_info->osd.fg1.w,lcd_info->osd.fg1.h);
	/* Foreground 0  */
	if ( lcd_info->osd.fg0.x >= lcd_info->panel.w )
		lcd_info->osd.fg0.x = lcd_info->panel.w;
	if ( lcd_info->osd.fg0.y >= lcd_info->panel.h )
		lcd_info->osd.fg0.y = lcd_info->panel.h;
	if ( lcd_info->osd.fg0.x + lcd_info->osd.fg0.w > lcd_info->panel.w )
		lcd_info->osd.fg0.w = lcd_info->panel.w - lcd_info->osd.fg0.x;
	if ( lcd_info->osd.fg0.y + lcd_info->osd.fg0.h > lcd_info->panel.h )
		lcd_info->osd.fg0.h = lcd_info->panel.h - lcd_info->osd.fg0.y;
#if 0 //allen del
	/* Foreground 1 */
	/* Case TVE ??? TVE 720x573 or 720x480*/
	if ( lcd_info->osd.fg1.x >= lcd_info->panel.w )
		lcd_info->osd.fg1.x = lcd_info->panel.w;
	if ( lcd_info->osd.fg1.y >= lcd_info->panel.h )
		lcd_info->osd.fg1.y = lcd_info->panel.h;
	if ( lcd_info->osd.fg1.x + lcd_info->osd.fg1.w > lcd_info->panel.w )
		lcd_info->osd.fg1.w = lcd_info->panel.w - lcd_info->osd.fg1.x;
	if ( lcd_info->osd.fg1.y + lcd_info->osd.fg1.h > lcd_info->panel.h )
		lcd_info->osd.fg1.h = lcd_info->panel.h - lcd_info->osd.fg1.y;
#endif

//	fg0_line_size = lcd_info->osd.fg0.w*((lcd_info->osd.fg0.bpp+7)/8);
	fg0_line_size = (lcd_info->osd.fg0.w*(lcd_info->osd.fg0.bpp)/8);
	fg0_line_size = ((fg0_line_size+3)>>2)<<2; /* word aligned */
	fg0_frm_size = fg0_line_size * lcd_info->osd.fg0.h;

	dprintf("fg0_frm_size = 0x%x\n",fg0_frm_size);

	fg1_line_size = lcd_info->osd.fg1.w*((lcd_info->osd.fg1.bpp+7)/8);
	fg1_line_size = ((fg1_line_size+3)>>2)<<2; /* word aligned */
	fg1_frm_size = fg1_line_size * lcd_info->osd.fg1.h;

	if ( lcd_info->osd.fg_change ) {
		if ( lcd_info->osd.fg_change & FG0_CHANGE_POSITION ) { /* F0 change position */
			REG_LCD_XYP0 = lcd_info->osd.fg0.y << 16 | lcd_info->osd.fg0.x;
		}
		if ( lcd_info->osd.fg_change & FG1_CHANGE_POSITION ) { /* F1 change position */
			REG_LCD_XYP1 = lcd_info->osd.fg1.y << 16 | lcd_info->osd.fg1.x;
		}

		/* set change */
		if ( !(lcd_info->osd.osd_ctrl & LCD_OSDCTRL_IPU) &&
		 (lcd_info->osd.fg_change != FG_CHANGE_ALL) )
			REG_LCD_OSDCTRL |= LCD_OSDCTRL_CHANGES;

		/* wait change ready???  maddrone open*/
//		while ( REG_LCD_OSDS & LCD_OSDS_READY )	/* fix in the future, Wolfgang, 06-20-2008 */
		dprintf("wait LCD_OSDS_READY\n");

		if ( lcd_info->osd.fg_change & FG0_CHANGE_SIZE ) { /* change FG0 size */
			if ( lcd_info->panel.cfg & LCD_CFG_TVEN ) { /* output to TV */
				dma0_desc0->cmd = dma0_desc1->cmd = (fg0_frm_size/4)/2;
				dma0_desc0->offsize = dma0_desc1->offsize
					= fg0_line_size/4;
				dma0_desc0->page_width = dma0_desc1->page_width
					= fg0_line_size/4;
				#ifdef TVOUT_2x
				if(tvout_640_480)
				dma0_desc1->databuf = virt_to_phys((void *)(lcd_frame0 + fg0_line_size));  //maddrone
				else
				dma0_desc1->databuf = virt_to_phys((void *)(lcd_frame01 + fg0_line_size));  //maddrone
				#else
				dma0_desc1->databuf = virt_to_phys((void *)(lcd_frame0 + fg0_line_size));
				#endif
				REG_LCD_DA0 = virt_to_phys(dma0_desc0); //tft
			}
			else {
				dma0_desc0->cmd = dma0_desc1->cmd = fg0_frm_size/4;
				dma0_desc0->offsize = dma0_desc1->offsize =0;
				dma0_desc0->page_width = dma0_desc1->page_width = 0;
			}

			dma0_desc0->desc_size = dma0_desc1->desc_size
				= lcd_info->osd.fg0.h << 16 | lcd_info->osd.fg0.w;
			REG_LCD_SIZE0 = (lcd_info->osd.fg0.h<<16)|lcd_info->osd.fg0.w;

		}

		if ( lcd_info->osd.fg_change & FG1_CHANGE_SIZE ) { /* change FG1 size*/
			if ( lcd_info->panel.cfg & LCD_CFG_TVEN ) { /* output to TV */
				dma1_desc0->cmd = dma1_desc1->cmd = (fg1_frm_size/4)/2;
				dma1_desc0->offsize = dma1_desc1->offsize = fg1_line_size/4;
				dma1_desc0->page_width = dma1_desc1->page_width = fg1_line_size/4;
				dma1_desc1->databuf = virt_to_phys((void *)(lcd_frame1 + fg1_line_size));  //maddrone
				REG_LCD_DA1 = virt_to_phys(dma0_desc1); //tft

			}
			else {
				dma1_desc0->cmd = dma1_desc1->cmd = fg1_frm_size/4;
				dma1_desc0->offsize = dma1_desc1->offsize = 0;
				dma1_desc0->page_width = dma1_desc1->page_width = 0;
			}

			dma1_desc0->desc_size = dma1_desc1->desc_size
				= lcd_info->osd.fg1.h << 16 | lcd_info->osd.fg1.w;
			REG_LCD_SIZE1 = lcd_info->osd.fg1.h << 16|lcd_info->osd.fg1.w;
		}

		//dma_cache_wback((unsigned int)(dma_desc_base), (DMA_DESC_NUM)*sizeof(struct jz4750_lcd_dma_desc));
		lcd_info->osd.fg_change = FG_NOCHANGE; /* clear change flag */
	}
}
/*
 * jz4750fb_set_mode(), set osd configure, resize foreground
 *
 */
static void jz4750fb_set_mode( struct jz4750lcd_info * lcd_info )
{
	jz4750fb_set_osd_mode(lcd_info);
	jz4750fb_foreground_resize(lcd_info);
}

/* initial dma descriptors */
static void jz4750fb_descriptor_init( struct jz4750lcd_info * lcd_info )
{
	unsigned int pal_size;

	switch ( lcd_info->osd.fg0.bpp ) {
	case 1:
		pal_size = 4;
		break;
	case 2:
		pal_size = 8;
		break;
	case 4:
		pal_size = 32;
		break;
	case 8:
	default:
		pal_size = 512;
	}

	pal_size /= 4;

	dma0_desc_palette 	= dma_desc_base + 0;
	dma0_desc0 		= dma_desc_base + 1;
	dma0_desc1 		= dma_desc_base + 2;
	dma0_desc_cmd0 		= dma_desc_base + 3; /* use only once */
	dma0_desc_cmd 		= dma_desc_base + 4;
	dma1_desc0 		= dma_desc_base + 5;
	dma1_desc1 		= dma_desc_base + 6;

	/*
	 * Normal TFT panel's DMA Chan0:
	 *	TO LCD Panel:
	 * 		no palette:	dma0_desc0 <<==>> dma0_desc0
	 * 		palette :	dma0_desc_palette <<==>> dma0_desc0
	 *	TO TV Encoder:
	 * 		no palette:	dma0_desc0 <<==>> dma0_desc1
	 * 		palette:	dma0_desc_palette --> dma0_desc0
	 * 				--> dma0_desc1 --> dma0_desc_palette --> ...
	 *
	 * SMART LCD TFT panel(dma0_desc_cmd)'s DMA Chan0:
	 *	TO LCD Panel:
	 * 		no palette:	dma0_desc_cmd <<==>> dma0_desc0
	 * 		palette :	dma0_desc_palette --> dma0_desc_cmd
	 * 				--> dma0_desc0 --> dma0_desc_palette --> ...
	 *	TO TV Encoder:
	 * 		no palette:	dma0_desc_cmd --> dma0_desc0
	 * 				--> dma0_desc1 --> dma0_desc_cmd --> ...
	 * 		palette:	dma0_desc_palette --> dma0_desc_cmd
	 * 				--> dma0_desc0 --> dma0_desc1
	 * 				--> dma0_desc_palette --> ...
	 * DMA Chan1:
	 *	TO LCD Panel:
	 * 		dma1_desc0 <<==>> dma1_desc0
	 *	TO TV Encoder:
	 * 		dma1_desc0 <<==>> dma1_desc1
	 */

#if defined(CONFIG_FB_JZ4750_SLCD)
	/* First CMD descriptors, use only once, cmd_num isn't 0 */
	dma0_desc_cmd0->next_desc 	= (unsigned int)virt_to_phys(dma0_desc0);
	dma0_desc_cmd0->databuf 	= (unsigned int)virt_to_phys((void *)lcd_cmdbuf);
	dma0_desc_cmd0->frame_id 	= (unsigned int)0x0da0cad0; /* dma0's cmd0 */
	dma0_desc_cmd0->cmd 		= LCD_CMD_CMD | 3; /* command */
	dma0_desc_cmd0->offsize 	= 0;
	dma0_desc_cmd0->page_width 	= 0;
	dma0_desc_cmd0->cmd_num 	= 3;

	/* Dummy Command Descriptor, cmd_num is 0 */
	dma0_desc_cmd->next_desc 	= (unsigned int)virt_to_phys(dma0_desc0);
	dma0_desc_cmd->databuf 		= 0;
	dma0_desc_cmd->frame_id 	= (unsigned int)0x0da000cd; /* dma0's cmd0 */
	dma0_desc_cmd->cmd 		= LCD_CMD_CMD | 0; /* dummy command */
	dma0_desc_cmd->cmd_num 		= 0;
	dma0_desc_cmd->offsize 		= 0;
	dma0_desc_cmd->page_width 	= 0;

	/* Palette Descriptor */
	dma0_desc_palette->next_desc 	= (unsigned int)virt_to_phys(dma0_desc_cmd0);
#else
	/* Palette Descriptor */
	dma0_desc_palette->next_desc 	= (unsigned int)virt_to_phys(dma0_desc0);
#endif
	dma0_desc_palette->databuf 	= (unsigned int)virt_to_phys((void *)lcd_palette);
	dma0_desc_palette->frame_id 	= (unsigned int)0xaaaaaaaa;
	dma0_desc_palette->cmd 		= LCD_CMD_PAL | pal_size; /* Palette Descriptor */

	/* DMA0 Descriptor0 */
	if ( lcd_info->panel.cfg & LCD_CFG_TVEN ) /* TVE mode */
		dma0_desc0->next_desc 	= (unsigned int)virt_to_phys(dma0_desc1);
	else{			/* Normal TFT LCD */
#if defined(CONFIG_FB_JZ4750_SLCD)
			dma0_desc0->next_desc = (unsigned int)virt_to_phys(dma0_desc_cmd);
#else
			dma0_desc0->next_desc = (unsigned int)virt_to_phys(dma0_desc0);
#endif
	}

#if 0
	//maddrone change here
	if(lcd_info->panel.cfg & LCD_CFG_TVEN) {
		;
	}
	else
	{
		dma0_desc0->databuf = virt_to_phys((void *)lcd_frame0);
		dma0_desc0->frame_id = (unsigned int)0x0000da00; /* DMA0'0 */
		//maddrone
		unsigned int frame_size0;
		frame_size0 = (400 * 360 * 16) >> 3;
		frame_size0 /= 4;
		dma0_desc0->cmd = frame_size0;
		dma0_desc0->desc_size = (360 << 16) | 400;
		dma0_desc0->offsize = 0;
		dma0_desc0->cmd_num = 0;
	}
#else
		dma0_desc0->databuf = virt_to_phys((void *)lcd_frame0);
		dma0_desc0->frame_id = (unsigned int)0x0000da00; /* DMA0'0 */
#endif

	/* DMA0 Descriptor1 */
	if ( lcd_info->panel.cfg & LCD_CFG_TVEN ) { /* TVE mode */

		dprintf("TV Enable Mode...\n");
		if (lcd_info->osd.fg0.bpp <= 8) /* load palette only once at setup */
			dma0_desc1->next_desc = (unsigned int)virt_to_phys(dma0_desc_palette);
		else
#if defined(CONFIG_FB_JZ4750_SLCD)  /* for smatlcd */
			dma0_desc1->next_desc = (unsigned int)virt_to_phys(dma0_desc_cmd);
#else
			dma0_desc1->next_desc = (unsigned int)virt_to_phys(dma0_desc0);
#endif
		dma0_desc1->frame_id = (unsigned int)0x0000da01; /* DMA0'1 */
	}

	if (lcd_info->osd.fg0.bpp <= 8) /* load palette only once at setup */
		REG_LCD_DA0 = virt_to_phys(dma0_desc_palette);
	else {
#if defined(CONFIG_FB_JZ4750_SLCD)  /* for smartlcd */
		REG_LCD_DA0 = virt_to_phys(dma0_desc_cmd0); //smart lcd
#else
		REG_LCD_DA0 = virt_to_phys(dma0_desc0); //tft
#endif
	}

	/* DMA1 Descriptor0 */
	if ( lcd_info->panel.cfg & LCD_CFG_TVEN ) /* TVE mode */
		dma1_desc0->next_desc = (unsigned int)virt_to_phys(dma1_desc1);
	else			/* Normal TFT LCD */
		dma1_desc0->next_desc = (unsigned int)virt_to_phys(dma1_desc0);

	dma1_desc0->databuf = virt_to_phys((void *)lcd_frame1);
	dma1_desc0->frame_id = (unsigned int)0x0000da10; /* DMA1'0 */

	/* DMA1 Descriptor1 */
	if ( lcd_info->panel.cfg & LCD_CFG_TVEN ) { /* TVE mode */
		dma1_desc1->next_desc = (unsigned int)virt_to_phys(dma1_desc0);
		dma1_desc1->frame_id = (unsigned int)0x0000da11; /* DMA1'1 */
	}

	REG_LCD_DA1 = virt_to_phys(dma1_desc0);	/* set Dma-chan1's Descripter Addrress */
	//dma_cache_wback_inv((unsigned int)(dma_desc_base), (DMA_DESC_NUM)*sizeof(struct jz4750_lcd_dma_desc));
	flush_cache_all();

}
static void jz4750fb_set_panel_mode( struct jz4750lcd_info * lcd_info )
{
	struct jz4750lcd_panel_t *panel = &lcd_info->panel;
#ifdef CONFIG_JZ4750D_VGA_DISPLAY
	REG_TVE_CTRL |= TVE_CTRL_DAPD;
	REG_TVE_CTRL &= ~( TVE_CTRL_DAPD1 | TVE_CTRL_DAPD2 | TVE_CTRL_DAPD3);
#endif
	/* set bpp */
	lcd_info->panel.ctrl &= ~LCD_CTRL_BPP_MASK;
	if ( lcd_info->osd.fg0.bpp == 1 )
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_1;
	else if ( lcd_info->osd.fg0.bpp == 2 )
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_2;
	else if ( lcd_info->osd.fg0.bpp == 4 )
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_4;
	else if ( lcd_info->osd.fg0.bpp == 8 )
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_8;
	else if ( lcd_info->osd.fg0.bpp == 15 )
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_16 | LCD_CTRL_RGB555;
	else if ( lcd_info->osd.fg0.bpp == 16 )
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_16 | LCD_CTRL_RGB565;
	else if ( lcd_info->osd.fg0.bpp > 16 && lcd_info->osd.fg0.bpp < 32+1 ) {
		lcd_info->osd.fg0.bpp = 32;
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_18_24;
	}
	else {
		dprintf("The BPP %d is not supported\n", lcd_info->osd.fg0.bpp);
		lcd_info->osd.fg0.bpp = 32;
		lcd_info->panel.ctrl |= LCD_CTRL_BPP_18_24;
	}

	lcd_info->panel.cfg |= LCD_CFG_NEWDES; /* use 8words descriptor always */

	REG_LCD_CTRL = lcd_info->panel.ctrl; /* LCDC Controll Register */
	REG_LCD_CFG = lcd_info->panel.cfg; /* LCDC Configure Register */
	REG_SLCD_CFG = lcd_info->panel.slcd_cfg; /* Smart LCD Configure Register */

	if ( lcd_info->panel.cfg & LCD_CFG_LCDPIN_SLCD ) /* enable Smart LCD DMA */
		REG_SLCD_CTRL = SLCD_CTRL_DMA_EN;

	switch ( lcd_info->panel.cfg & LCD_CFG_MODE_MASK ) {
	case LCD_CFG_MODE_GENERIC_TFT:
	case LCD_CFG_MODE_INTER_CCIR656:
	case LCD_CFG_MODE_NONINTER_CCIR656:
	case LCD_CFG_MODE_SLCD:
	default:		/* only support TFT16 TFT32, not support STN and Special TFT by now(10-06-2008)*/
		REG_LCD_VAT = (((panel->blw + panel->w + panel->elw + panel->hsw)) << 16) | (panel->vsw + panel->bfw + panel->h + panel->efw);
		REG_LCD_DAH = ((panel->hsw + panel->blw) << 16) | (panel->hsw + panel->blw + panel->w);
		REG_LCD_DAV = ((panel->vsw + panel->bfw) << 16) | (panel->vsw + panel->bfw + panel->h);
		REG_LCD_HSYNC = (0 << 16) | panel->hsw;
		REG_LCD_VSYNC = (0 << 16) | panel->vsw;
		break;
	}
}
/**
 * @brief jz4750fb_deep_set_mode
 *
 * @param lcd_info
 */
static void jz4750fb_deep_set_mode( struct jz4750lcd_info * lcd_info )
{
	__lcd_clr_ena();

	lcd_info->osd.fg_change = FG_CHANGE_ALL; /* change FG0, FG1 size, postion??? */
	jz4750fb_descriptor_init(lcd_info);
	jz4750fb_set_panel_mode(lcd_info);
	jz4750fb_set_mode(lcd_info);
	jz4750fb_change_clock(lcd_info);

#if defined(CONFIG_FB_JZ4750_SLCD)
	__lcd_set_ena();	/* enable lcdc */
#endif

}

void print_lcdc_registers(void)	/* debug */
{
#if 0
	/* LCD Controller Resgisters */
	dprintf("REG_LCD_CFG:\t0x%08x\n", REG_LCD_CFG);
	dprintf("REG_LCD_CTRL:\t0x%08x\n", REG_LCD_CTRL);
	dprintf("REG_LCD_STATE:\t0x%08x\n", REG_LCD_STATE);
	dprintf("REG_LCD_OSDC:\t0x%08x\n", REG_LCD_OSDC);
	dprintf("REG_LCD_OSDCTRL:\t0x%08x\n", REG_LCD_OSDCTRL);
	dprintf("REG_LCD_OSDS:\t0x%08x\n", REG_LCD_OSDS);
	dprintf("REG_LCD_BGC:\t0x%08x\n", REG_LCD_BGC);
	dprintf("REG_LCD_KEK0:\t0x%08x\n", REG_LCD_KEY0);
	dprintf("REG_LCD_KEY1:\t0x%08x\n", REG_LCD_KEY1);
	dprintf("REG_LCD_ALPHA:\t0x%08x\n", REG_LCD_ALPHA);
	dprintf("REG_LCD_IPUR:\t0x%08x\n", REG_LCD_IPUR);
	dprintf("REG_LCD_VAT:\t0x%08x\n", REG_LCD_VAT);
	dprintf("REG_LCD_DAH:\t0x%08x\n", REG_LCD_DAH);
	dprintf("REG_LCD_DAV:\t0x%08x\n", REG_LCD_DAV);
	dprintf("REG_LCD_XYP0:\t0x%08x\n", REG_LCD_XYP0);
	dprintf("REG_LCD_XYP1:\t0x%08x\n", REG_LCD_XYP1);
	dprintf("REG_LCD_SIZE0:\t0x%08x\n", REG_LCD_SIZE0);
	dprintf("REG_LCD_SIZE1:\t0x%08x\n", REG_LCD_SIZE1);
	dprintf("REG_LCD_RGBC\t0x%08x\n", REG_LCD_RGBC);
	dprintf("REG_LCD_VSYNC:\t0x%08x\n", REG_LCD_VSYNC);
	dprintf("REG_LCD_HSYNC:\t0x%08x\n", REG_LCD_HSYNC);
	dprintf("REG_LCD_PS:\t0x%08x\n", REG_LCD_PS);
	dprintf("REG_LCD_CLS:\t0x%08x\n", REG_LCD_CLS);
	dprintf("REG_LCD_SPL:\t0x%08x\n", REG_LCD_SPL);
	dprintf("REG_LCD_REV:\t0x%08x\n", REG_LCD_REV);
	dprintf("REG_LCD_IID:\t0x%08x\n", REG_LCD_IID);
	dprintf("REG_LCD_DA0:\t0x%08x\n", REG_LCD_DA0);
	dprintf("REG_LCD_SA0:\t0x%08x\n", REG_LCD_SA0);
	dprintf("REG_LCD_FID0:\t0x%08x\n", REG_LCD_FID0);
	dprintf("REG_LCD_CMD0:\t0x%08x\n", REG_LCD_CMD0);
	dprintf("REG_LCD_OFFS0:\t0x%08x\n", REG_LCD_OFFS0);
	dprintf("REG_LCD_PW0:\t0x%08x\n", REG_LCD_PW0);
	dprintf("REG_LCD_CNUM0:\t0x%08x\n", REG_LCD_CNUM0);
	dprintf("REG_LCD_DESSIZE0:\t0x%08x\n", REG_LCD_DESSIZE0);
	dprintf("REG_LCD_DA1:\t0x%08x\n", REG_LCD_DA1);
	dprintf("REG_LCD_SA1:\t0x%08x\n", REG_LCD_SA1);
	dprintf("REG_LCD_FID1:\t0x%08x\n", REG_LCD_FID1);
	dprintf("REG_LCD_CMD1:\t0x%08x\n", REG_LCD_CMD1);
	dprintf("REG_LCD_OFFS1:\t0x%08x\n", REG_LCD_OFFS1);
	dprintf("REG_LCD_PW1:\t0x%08x\n", REG_LCD_PW1);
	dprintf("REG_LCD_CNUM1:\t0x%08x\n", REG_LCD_CNUM1);
	dprintf("REG_LCD_DESSIZE1:\t0x%08x\n", REG_LCD_DESSIZE1);
	dprintf("==================================\n");
	dprintf("REG_LCD_VSYNC:\t%d:%d\n", REG_LCD_VSYNC>>16, REG_LCD_VSYNC&0xfff);
	dprintf("REG_LCD_HSYNC:\t%d:%d\n", REG_LCD_HSYNC>>16, REG_LCD_HSYNC&0xfff);
	dprintf("REG_LCD_VAT:\t%d:%d\n", REG_LCD_VAT>>16, REG_LCD_VAT&0xfff);
	dprintf("REG_LCD_DAH:\t%d:%d\n", REG_LCD_DAH>>16, REG_LCD_DAH&0xfff);
	dprintf("REG_LCD_DAV:\t%d:%d\n", REG_LCD_DAV>>16, REG_LCD_DAV&0xfff);
	dprintf("==================================\n");

	/* Smart LCD Controller Resgisters */
	dprintf("REG_SLCD_CFG:\t0x%08x\n", REG_SLCD_CFG);
	dprintf("REG_SLCD_CTRL:\t0x%08x\n", REG_SLCD_CTRL);
	dprintf("REG_SLCD_STATE:\t0x%08x\n", REG_SLCD_STATE);
	dprintf("==================================\n");


	dprintf("==================================\n");
	if ( dma_desc_base != NULL ) {
		unsigned int * pii = (unsigned int *)dma_desc_base;
		int i, j;
		for (j=0;j< DMA_DESC_NUM ; j++) {
			dprintf("dma_desc%d(0x%08x):\n", j, (unsigned int)pii);
			for (i =0; i<8; i++ ) {
				dprintf("\t\t0x%08x\n", *pii++);
			}
		}
	}
#endif
}

void lcd_rinit(void *lcdbase)
{
	jz4750_lcd_info->osd.fg_change = FG_CHANGE_ALL; /* change FG0, FG1 size, postion??? */
	jz4750fb_descriptor_init(jz4750_lcd_info);
	jz4750fb_set_mode(jz4750_lcd_info);
	jz4750fb_change_clock(jz4750_lcd_info);
}

#define COLOR_TO_MTK_COLOR_SIMUL(color) ((((color) >> 19) & 0x1f) << 11) |((((color) >> 10) & 0x3f) << 5)  |(((color) >> 3) & 0x1f)
#define RGB565(R,G,B) ((R & 0x1f) << 11) |((G & 0x3f) << 5)  |(B & 0x1f)

// void draw_logo(unsigned short *src, uint16_t iw, uint16_t ih)
void draw_logo(unsigned short *src, uint16_t src_w, uint16_t src_h, int16_t dst_x, int16_t dst_y)
{
	int l, x, y;

	int16_t dst_w = jz4750_lcd_info->osd.fg1.w;
	int16_t dst_h = jz4750_lcd_info->osd.fg1.h;
	uint8_t factor = (dst_w == 320 && dst_h == 480) ? 2 : 1; // RG 320x480 hack
	unsigned short *dst = (unsigned short *)lcd_frame0;

	if (dst_y < 0)
		dst += (dst_h - (src_h * factor)) * dst_w / 2; // center vertically
	else
		dst += dst_y * dst_w * factor; // y offset

	if (dst_x < 0)
		dst += (dst_w - src_w) / 2; // center horizontally
	else
		dst += dst_x; // x offset

	// for (i = 0; i < w*h; i++) *dst++ = *src++;
	for (y = 0; y < src_h; y++) {
		for (l = 0; l < factor; l++) { // double h line
			for (x = 0; x < src_w; x++) {
				*dst++ = src[x + y * src_w];
			}
			dst += (dst_w - src_w);
		}
	}

	flush_cache_all();
}

void draw_battery(int charge_count)
{
	switch (charge_count) {
		case 2:
			draw_logo(battery_2, battery_2_w, battery_2_h, -1, -1);
			break;
		case 1:
			draw_logo(battery_1, battery_1_w, battery_1_h, -1, -1);
			break;
		default:
			draw_logo(battery_0, battery_0_w, battery_0_h, -1, -1);
	}
}

void lcd_clean_frame()
{
	int i, w, h;
	unsigned short *p = (unsigned short *)lcd_frame0;

	h = jz4750_lcd_info->osd.fg1.h;
	w = jz4750_lcd_info->osd.fg1.w;

	for (i = 0; i < w * h; i++)
		*p++ = 0x0;

	flush_cache_all();
}

static void ctrl_enable(void)
{
	REG_LCD_STATE = 0;
	__lcd_slcd_special_on();
	__lcd_clr_dis();
	__lcd_set_ena();
}

static void ctrl_disable(void)
{
	int cnt ;

	if ( jz4750_lcd_info->panel.cfg & LCD_CFG_LCDPIN_SLCD)/* Smart lcd only support quick disable */
		__lcd_clr_ena();
	else
	{
		cnt = 528000 * 2;//528000*30
		__lcd_set_dis();
		while(!__lcd_disable_done() && cnt){
			cnt --;
		}
		if (cnt == 0)
			dprintf("LCD quick disable timeout!\n");
		REG_LCD_STATE &= ~LCD_STATE_LDD;
	}
}

static void screen_on(void)
{
	__lcd_display_on();
	mdelay(200);
	__lcd_set_backlight_level(BACKLIGHT_LEVEL);
	return;
}

static void screen_off(void)
{
	__lcd_close_backlight();
	__lcd_display_off();
}

static void lcd_gpio_init(void)
{
	__lcd_display_pin_init();

	if (jz4750_lcd_info->panel.cfg & LCD_CFG_LCDPIN_SLCD)
		__gpio_as_lcd_8bit();
	else if (jz4750_lcd_info->panel.cfg & LCD_CFG_MODE_SERIAL_TFT)
	{
#if defined(CONFIG_JZ4760_LCD_SNK) || defined(CONFIG_JZ4760_LCD_RG_V10) || defined(CONFIG_JZ4760_LCD_RG_V21) || defined(CONFIG_JZ4760_LCD_RG_V30) || defined(CONFIG_JZ4760_LCD_RG_IPS)
		//__gpio_as_lcd_8bit + pclk
		REG_GPIO_PXFUNS(2) = 0x000c31fc;
		REG_GPIO_PXTRGC(2) = 0x000c31fc;
		REG_GPIO_PXSELC(2) = 0x000c31fc;
		REG_GPIO_PXPES(2)  = 0x000c31fc;
#endif

		//DE io set 1
		__gpio_as_func0(32*2+9);
		__gpio_as_output(32*2+9);
		__gpio_set_pin(32*2+9);
	}
	else
	{
		/* gpio init __gpio_as_lcd */
		if (jz4750_lcd_info->panel.cfg & LCD_CFG_MODE_TFT_16BIT)
			__gpio_as_lcd_16bit();
		else if (jz4750_lcd_info->panel.cfg & LCD_CFG_MODE_TFT_24BIT)
			__gpio_as_lcd_24bit();
		else if (jz4750_lcd_info->panel.cfg & LCD_CFG_MODE_TFT_18BIT)
			__gpio_as_lcd_18bit();
	}
}

static void lcd_set_bpp_to_ctrl_bpp(void)
{
	if ( jz4750_lcd_info->osd.fg0.bpp > 16 &&
			jz4750_lcd_info->osd.fg0.bpp < 32 ) {
		jz4750_lcd_info->osd.fg0.bpp = 32;
	}
	switch ( jz4750_lcd_info->osd.fg1.bpp ) {
		case 15:
		case 16:
			break;
		case 17 ... 32:
			jz4750_lcd_info->osd.fg1.bpp = 32;
			break;
		default:
			dprintf("jz4750fb fg1 not support bpp(%d), force to 32bpp\n",
					jz4750_lcd_info->osd.fg1.bpp);
			jz4750_lcd_info->osd.fg1.bpp = 32;
	}

	return;

}

static void slcd_init(void)
{
	/* Configure SLCD module for setting smart lcd control registers */
#if defined(CONFIG_FB_JZ4750_SLCD)
	__lcd_as_smart_lcd();
	__slcd_disable_dma();
#endif
}

extern int usb_detect();
extern int get_battery_mv(void);
extern void power_off_ub(void);

void lcd_ctrl_init (void *lcdbase)
{
	void * gl_mem_addr = 0x83000000;

	int mv, i, charge_count = 0;

	screen_off();
	ctrl_disable();

	lcd_gpio_init();
	slcd_init();

	lcd_set_bpp_to_ctrl_bpp();

	jz_lcd_init_mem(gl_mem_addr, &panel_info);
	jz4750fb_deep_set_mode(jz4750_lcd_info);

	lcd_clean_frame();
	// draw_logo(bootlogo, bootlogo_w, bootlogo_h, -1, -1); //must draw frambuffer early

#if !defined(CONFIG_FB_JZ4750_SLCD)
	lcd_rinit(lcd_base);
#endif
	ctrl_enable();

	screen_on();

	mv = get_battery_mv();

	if (mv < WARN_BATTERY_DATA) { // warn battery
		i = 0;
		do {
			draw_battery(charge_count);
			charge_count = (charge_count + 1) % BATTERY_STEPS;

			mdelay(1500);

			if (++i >= BATTERY_STEPS * 5 || (mv < LOW_BATTERY_DATA && !usb_detect())) goto poweroff;

			mv = get_battery_mv();

		} while (mv < LOW_BATTERY_DATA && usb_detect()); // draw charging
	}

	lcd_clean_frame();

	draw_logo(bootlogo, bootlogo_w, bootlogo_h, -1, -1);

#ifdef CFG_SUPPORT_RECOVERY_KEY
	if (is_recovery_keys_pressed())
		draw_logo(sd, sd_w, sd_h, 2, 2);
#endif

	return;

	poweroff:
		lcd_clean_frame();
		power_off_ub();
}

/*----------------------------------------------------------------------*/
#if LCD_BPP == LCD_COLOR8
void
lcd_setcolreg (ushort regno, ushort red, ushort green, ushort blue)
{
}
#endif
/*----------------------------------------------------------------------*/

#if LCD_BPP == LCD_MONOCHROME
static
void lcd_initcolregs (void)
{
}
#endif

/*----------------------------------------------------------------------*/

/*
 * Before enabled lcd controller, lcd registers should be configured correctly.
 */

void lcd_enable (void)
{
#if 0
	__lcd_clr_dis();
	__lcd_set_ena();
#endif
}


/************************************************************************/
static int jz_lcd_init_mem(void *lcdbase, vidinfo_t *vid)
{
	u_long palette_mem_size;
	int j = 0,i = 0;
	struct jz_fb_info *fbi = &vid->jz_fb;
	int fb_size;

	dprintf("w = %d h = %d  bpp = %d\n",vid->vl_row,vid->vl_col,vid->vl_bpix);

	fb_size = vid->vl_row * (vid->vl_col * NBITS (vid->vl_bpix)) / 8;
	fbi->screen = (u_long)lcdbase;
	lcd_frame0 = (u_long)lcdbase;
	fbi->palette_size = 256;
	palette_mem_size = fbi->palette_size * sizeof(u16);

	debug("palette_mem_size = 0x%08lx\n", (u_long) palette_mem_size);

	fbi->palette = (u_long)lcdbase + fb_size + PAGE_SIZE - palette_mem_size;
	lcd_palette =  fbi->palette;
	dma_desc_base = (struct jz4750_lcd_dma_desc *)((void*)lcd_palette + ((palette_mem_size+3)/4)*4);

	if ( dma_desc_base != NULL )
	{
		unsigned int * pii = (unsigned int *)dma_desc_base;
		for (j=0;j< DMA_DESC_NUM ; j++)
		{
			dprintf("dma_desc%d(0x%08x):\n", j, (unsigned int)pii);
			for (i =0; i<8; i++ )
				*pii++ = 0;
		}
	}

	return 0;
}

#endif /* CONFIG_LCD */
#endif //CONFIG_JZ4760
