#ifndef _SDRAM_H
#define _SDRAM_H


/*--------------------------------------------------------------------------------
 * SDRAM info
 */
#if 1
#  define CONFIG_NR_DRAM_BANKS	1       /* SDRAM BANK Number: 1, 2*/
/* SDRAM paramters */

#  define SDRAM_BW16		1	/* Data bus width: 0-32bit, 1-16bit */
#  define SDRAM_BANK4		1	/* Banks each chip: 0-2bank, 1-4bank */
#  define SDRAM_ROW		13	/* Row address: 11 to 13 */
#  define SDRAM_COL		10	/* Column address: 8 to 12 */
//#  define SDRAM_CASL		2	/* CAS latency: 2 or 3 */
#  define SDRAM_CASL		3	/* CAS latency: 2 or 3 */

/* SDRAM Timings, unit: ns */
#if 1
#  define SDRAM_TRAS		44	/* RAS# Active Time */
#  define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#  define SDRAM_TPC		20	/* RAS# Precharge Time */
#  define SDRAM_TRWL		8	/* Write Latency Time */
#  define SDRAM_TREF	        7812	/* Refresh period: 4096 refresh cycles/64ms */
#else
#  define SDRAM_TRAS		48	/* RAS# Active Time */
#  define SDRAM_RCD		28	/* RAS# to CAS# Delay */
#  define SDRAM_TPC		28	/* RAS# Precharge Time */
#  define SDRAM_TRWL		10	/* Write Latency Time */
#  define SDRAM_TREF	        30000	/* Refresh period: 4096 refresh cycles/64ms */

#endif
#else
#  define CONFIG_NR_DRAM_BANKS	1       /* SDRAM BANK Number: 1, 2*/
/* SDRAM paramters */

#  define SDRAM_BW16		1	/* Data bus width: 0-32bit, 1-16bit */
#  define SDRAM_BANK4		1	/* Banks each chip: 0-2bank, 1-4bank */
#  define SDRAM_ROW		13	/* Row address: 11 to 13 */
#  define SDRAM_COL		10	/* Column address: 8 to 12 */
//#  define SDRAM_CASL		2	/* CAS latency: 2 or 3 */
#  define SDRAM_CASL		3	/* CAS latency: 2 or 3 */

/* SDRAM Timings, unit: ns */
#if 0
#  define SDRAM_TRAS		45	/* RAS# Active Time */
#  define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#  define SDRAM_TPC		20	/* RAS# Precharge Time */
#  define SDRAM_TRWL		7	/* Write Latency Time */
#  define SDRAM_TREF	        7812	/* Refresh period: 4096 refresh cycles/64ms */
#else
#  define SDRAM_TRAS		48	/* RAS# Active Time */
#  define SDRAM_RCD		26	/* RAS# to CAS# Delay */
#  define SDRAM_TPC		26	/* RAS# Precharge Time */
#  define SDRAM_TRWL		8	/* Write Latency Time */
#  define SDRAM_TREF	        30000	/* Refresh period: 4096 refresh cycles/64ms */
#endif
#endif


#endif /* _SDRAM_H */




