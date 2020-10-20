/*
 * ASIX AX88796C Ethernet
 * (C) Copyright 2010
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 */

#include <asm/types.h>
#include <config.h>


#define CONFIG_DRIVER_AX88796C
#ifdef CONFIG_DRIVER_AX88796C

enum watchdog_state {
	chk_link = 0,
	chk_cable,
};

struct ax88796c_private {
	unsigned short seq_num;
	enum watchdog_state w_state;
	unsigned long w_ticks;
#define AX88796C_WATCHDOG_RESTART	7
	unsigned long timeout;
	unsigned link:1;
};

/* Sturctures declaration */

/* Tx headers structure */
struct tx_sop_header {
	/* bit 15-11: flags, bit 10-0: packet length */
	u16 flags_pktlen;
	/* bit 15-11: sequence number, bit 11-0: packet length bar */
	u16 seqnum_pktlenbar; 
} __attribute__((packed));

struct tx_segment_header {
	/* bit 15-14: flags, bit 13-11: segment number, bit 10-0: segment length */
	u16 flags_seqnum_seglen;
	/* bit 15-14: end offset, bit 13-11: start offset */
	/* bit 10-0: segment length bar */
	u16 eo_so_seglenbar;
} __attribute__((packed));

struct tx_eop_header {
	/* bit 15-11: sequence number, bit 10-0: packet length */
	u16 seqnum_pktlen;
	/* bit 15-11: sequence number bar, bit 10-0: packet length bar */
	u16 seqnumbar_pktlenbar;
} __attribute__((packed));

struct tx_header {
	struct tx_sop_header sop;
	struct tx_segment_header seg;
	struct tx_eop_header eop;
} __attribute__((packed));

struct rx_hdr1 {
	u16 flags_len;
	u16 seq_lenbar;
}__attribute__((packed));

struct rx_header {
	struct rx_hdr1 hdr1;
	u16 hdr2;
} __attribute__((packed));


#if (CONFIG_AX88796B_PIN_COMPATIBLE)
#define AX_SHIFT(x)				((x) << 1)
#else
#define AX_SHIFT(x)				((x) << 0)
#endif

#define PHY_ID					0x10
#define MII_ADVERTISE				0x05E1
#define PG_HOST_WAKEUP				0x1F
#if (CONFIG_AX88796B_PIN_COMPATIBLE)
	#define DATA_PORT_ADDR			(0x0020 >> 3)
#else
	#define DATA_PORT_ADDR			(0x0020)
#endif
/*TX Header*/
#define TX_HDR_SEG_FS				0x8000
#define TX_HDR_SEG_LS				0x4000
#define TX_HDR_SOP_PKTLENBAR			0x07FF
#define TX_HDR_SEQNUM(x)			(x << 11)
/*RX Header*/
#define RX_HDR_ERROR 				0x7000
#define RX_HDR_LEN 				0x07FF

/*Register */
#define P0_PSR				AX_SHIFT(0x00)
	#define PSR_PAGE_MASK			0xFFF8
	#define PAGE0 				0x00
	#define PAGE1				0x01
	#define PAGE2				0x02
	#define PAGE3				0x03
	#define PAGE4				0x04
	#define PSR_BUS_MASK			0xFF8F
	#define PSR_RESET			(0 << 15)
	#define PSR_RESET_CLR			(1 << 15)
	#define PSR_DEV_READY			(1 << 7)
#define P0_BOR				AX_SHIFT(0x02)
#define P0_FER				AX_SHIFT(0x04)
	#define FER_DROPCRC			(0xFFFD)
	#define FER_WSWAP			(1 << 8)
	#define FER_BSWAP			(1 << 9)
	#define FER_IRQHIGH		 	(1 << 10)
	#define FER_IRQTYPE	 		(1 << 11)
	#define FER_RXEN			(1 << 14)
	#define FER_TXEN			(1 << 15)
#define P0_ISR				AX_SHIFT(0x06)
	#define ISR_RXPCT			(1 << 0)
	#define ISR_TXERR			(1 << 8)
	#define ISR_LINK			(1 << 9)
#define P0_PSCR				AX_SHIFT(0x0C)
	#define PSCR_PS_MASK		(0xFFF0)
	#define PSCR_PS_D0		(0)
	#define PSCR_PS_D1		(1 << 0)
	#define PSCR_PS_D2		(1 << 1)
	#define PSCR_FPS		(1 << 3) /* Enable fiber mode PS */
	#define PSCR_SWPS		(1 << 4) /* Enable software PS control */
	#define PSCR_WOLPS		(1 << 5) /* Enable WOL PS */
	#define PSCR_SWWOL		(1 << 6) /* Enable software select WOL PS */
	#define PSCR_PHYOSC		(1 << 7) /* Internal PHY OSC control */
	#define PSCR_FOFEF		(1 << 8) /* Force PHY generate FEF */
	#define PSCR_FOF		(1 << 9) /* Force PHY in fiber mode */
	#define PSCR_PHYPD		(1 << 10) /* PHY power down. Active high */
	#define PSCR_PHYRST		(1 << 11) /* PHY reset signal. Active low */
	#define PSCR_PHYCSIL		(1 << 12) /* PHY cable energy detect */
	#define PSCR_PHYCOFF		(1 << 13) /* PHY cable off */
	#define PSCR_PHYLINK		(1 << 14) /* PHY link status */
	#define PSCR_EEPOK		(1 << 15) /* EEPROM load complete */
#define P0_MACCR				0x0E
	#define MACCR_RXPATH			1 << 0
#define P0_TSNR				AX_SHIFT(0x12)
	#define TSNR_PKT_CNT(x)			(x << 8)
	#define TSNR_TXB_START			(1 << 15)
	#define TSNR_TXB_REINIT			(1 << 14)
	#define TXNR_TXB_IDLE			(1 << 6)
#define P0_RTDPR			AX_SHIFT(0x14)
#define P0_RXBCR1			AX_SHIFT(0x16)
	#define RXBCR1_RXB_DISCARD		(1 << 14)
	#define RXBCR1_RXB_START		(1 << 15)
#define P0_RXBCR2			AX_SHIFT(0x18)
	#define RXBCR2_PKT_MASK		(0x00FF)
	#define RXBCR2_RXB_READY		(1 << 13)
	#define RXBCR2_RXB_IDLE			(1 << 14)
	#define RXBCR2_RXB_REINIT		(1 << 15)
#define P0_RTWCR	AX_SHIFT(0x1A)
	#define RTWCR_RXWC_MASK		(0x3FFF)
	#define RTWCR_RX_LATCH		(1 << 15)
#define P0_RCPHR			AX_SHIFT(0x1C)
#define P1_RPCTR			AX_SHIFT(0x02)
#define P1_RXBSPCR			AX_SHIFT(0x10)

#define P2_POOLCR			AX_SHIFT(0x04)
	#define POOLCR_POLL_EN			(1 << 0)
	#define POOLCR_POLL_BMCR		(1 << 2)
	#define POOLCR_PHYID(x)			((x) << 8)

#define P2_MRDR				AX_SHIFT(0x08)
#define P2_MRCR				AX_SHIFT(0x0A)
	#define MRCR_RADDR(x)			((x) & 0x1F)
	#define MRCR_FADDR(x)			(((x) & 0x1F) << 8)
	#define MRCR_VALID			(1 << 13)
	#define MRCR_READ			(1 << 14)
	#define MRCR_WRITE			(1 << 15)

#define P2_LCR0				AX_SHIFT(0x0C)
	#define LCR_LED0_EN			(1 << 0)
	#define LCR_LED0_DUPLEX			(1 << 2)
	#define LCR_LED1_EN			(1 << 8)
	#define LCR_LED1_100MODE		(1 << 9)
#define P2_LCR1				AX_SHIFT(0x0E)
	#define LCR_LED2_MASK			0XFF00
	#define LCR_LED2_EN			(1 << 0)
	#define LCR_LED2_LINK			(1 << 3)
#define P2_RXCR				AX_SHIFT(0x16)
	#define RXCR_PROMISCUS			0x0001
	#define RXCR_BROADCAST			(1 << 3)
#define P3_MACASR0			AX_SHIFT(0x02)
#define P3_MACASR1			AX_SHIFT(0x04)
#define P3_MACASR2			AX_SHIFT(0x06)

#define P4_COERCR0			AX_SHIFT(0x12)
	#define COERCR0_ALLON			0x1FFF
#define P4_COERCR1			AX_SHIFT(0x14)
	#define COERCR1_DROP_ALL		0x7FFF

#endif /*end of CONFIG_DRIVER_AX88796B*/
