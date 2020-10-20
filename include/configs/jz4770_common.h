/*
 * The file define all the common macro for the board based on the JZ4760
 */


#ifndef __JZ4770_COMMON_H__
#define __JZ4770_COMMON_H__


#define __CFG_EXTAL     (CFG_EXTAL / 1000000)
#define __CFG_PLL_OUT   (CFG_CPU_SPEED / 1000000)

#ifdef CFG_PLL1_FRQ
    #define __CFG_PLL1_OUT  ((CFG_PLL1_FRQ)/1000000)    /* Set PLL1 default: 240MHz */
#else
    #define __CFG_PLL1_OUT  (432)                       /* Set PLL1 default: 432MHZ  UHC:48MHZ TVE:27MHZ */
#endif

/*pll_0*/ 
#if (__CFG_PLL_OUT > 1200)
	#error "PLL output can NOT more than 1200MHZ"
#elif (__CFG_PLL_OUT > 600)
	#define __PLL_BS          1
	#define __PLL_OD          0
#elif (__CFG_PLL_OUT > 300)
	#define __PLL_BS          0
	#define __PLL_OD          0
#elif (__CFG_PLL_OUT > 155)
	#define __PLL_BS          0
	#define __PLL_OD          1
#elif (__CFG_PLL_OUT > 76)
	#define __PLL_BS          0
	#define __PLL_OD          2
#elif (__CFG_PLL_OUT > 47)
	#define __PLL_BS          0
	#define __PLL_OD          3
#else
	#error "PLL ouptput can NOT less than 48"
#endif

#define __PLL_NO		0
#define NR 			(__PLL_NO + 1)
#define NO 			(0x1 << __PLL_OD)
#define __PLL_MO		(((__CFG_PLL_OUT / __CFG_EXTAL) * NR * NO) - 1)
#define NF 			(__PLL_MO + 1)
#define FOUT			(__CFG_EXTAL * NF / NR / NO)

#if ((__CFG_EXTAL / NR > 50) || (__CFG_EXTAL / NR < 10))
	#error "Can NOT set the value to PLL_N"
#endif

#if ((__PLL_MO > 127) || (__PLL_MO < 1))
	#error "Can NOT set the value to PLL_M"
#endif

#if (__PLL_BS == 1)
	#if (((FOUT * NO) > 1200) || ((FOUT * NO) < 500))
		#error "FVCO check failed : BS = 1"
	#endif
#elif (__PLL_BS == 0)
	#if (((FOUT * NO) > 600) || ((FOUT * NO) < 300))
		#error "FVCO check failed : BS = 0"
	#endif
#endif

#define CPCCR_M_N_OD	((__PLL_MO << 24) | (__PLL_NO << 18) | (__PLL_OD << 16) | (__PLL_BS << 31))


/**************************************************************************************************************/

#if (__CFG_PLL1_OUT > 1200)
	#error "PLL1 output can NO1T more than 1200MHZ"
#elif (__CFG_PLL1_OUT > 600)
	#define __PLL1_BS          1
	#define __PLL1_OD          0
#elif (__CFG_PLL1_OUT > 300)
	#define __PLL1_BS          0
	#define __PLL1_OD          0
#elif (__CFG_PLL1_OUT > 155)
	#define __PLL1_BS          0
	#define __PLL1_OD          1
#elif (__CFG_PLL1_OUT > 76)
	#define __PLL1_BS          0
	#define __PLL1_OD          2
#elif (__CFG_PLL1_OUT > 47)
	#define __PLL1_BS          0
	#define __PLL1_OD          3
#else
	#error "PLL1 ouptput can NOT less than 48"
#endif

#define __PLL1_NO1		0
#define NR1 			(__PLL1_NO1 + 1)
#define NO1 			(0x1 << __PLL1_OD)
#define __PLL1_MO		(((__CFG_PLL1_OUT / __CFG_EXTAL) * NR1 * NO1) - 1)
#define NF1 			(__PLL1_MO + 1)
#define FOUT1			(__CFG_EXTAL * NF1 / NR1 / NO1)

#if ((__CFG_EXTAL / NR1 > 50) || (__CFG_EXTAL / NR1 < 10))
	#error "Can NOT set the value to PLL1_N"
#endif

#if ((__PLL1_MO > 127) || (__PLL1_MO < 1))
	#error "Can NOT set the value to PLL1_M"
#endif

#if (__PLL1_BS == 1)
	#if (((FOUT1 * NO1) > 1200) || ((FOUT1 * NO1) < 500))
		#error "FVCO1 check failed : BS1 = 1"
	#endif
#elif (__PLL1_BS == 0)
	#if (((FOUT1 * NO1) > 600) || ((FOUT1 * NO1) < 300))
		#error "FVCO1 check failed : BS1 = 0"
	#endif
#endif

#define CPCCR1_M_N_OD	((__PLL1_MO << 24) | (__PLL1_NO1 << 18) | (__PLL1_OD << 16) | (__PLL1_BS << 31))

#endif /* __JZ4770_COMMON_H__ */
