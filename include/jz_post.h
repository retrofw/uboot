#ifndef _JZ_POST_H
#define _JZ_POST_H

#ifndef	__ASSEMBLY__
#include <common.h>
#endif

#ifdef CONFIG_POST

#define POST_RAM		0x0200	/* test runs in RAM */
#define POST_MANUAL		0x0400	/* test runs on diag command */

union cpm_scratch_pad {
	unsigned int value;
	struct {
		unsigned int curr:7;
		unsigned int curr_valid:1;
		unsigned int curr_prev_valid:1;

		unsigned int first:7;
		unsigned int first_valid:1;

		unsigned int flags:3;

		unsigned int magic:12;
	} ddr_delay;
};

#define JZ_POST_MAGIC		0xDEA
#define JZ_POST_MEM_AUTO	0x1
#define JZ_POST_MEM_MANUAL	0x2

#define get_cpm_scratch_pad()	(*((volatile union cpm_scratch_pad *)0xB0000034))
#define set_cpm_scratch_pad(__value)					\
	do {								\
		*(volatile unsigned int *)0xB0000038 = 0x00005a5a;	\
		*(volatile union cpm_scratch_pad*)0xB0000034 = __value;	\
		*(volatile unsigned int *)0xB0000038 = 0x0000a5a5;	\
	} while(0)

#define ddr_spad_set_first()						\
	do {								\
		union cpm_scratch_pad ______cpm_spad = get_cpm_scratch_pad(); \
		if (______cpm_spad.ddr_delay.magic == JZ_POST_MAGIC) {	\
			if ((______cpm_spad.ddr_delay.first_valid == 0) && \
			    (______cpm_spad.ddr_delay.curr_valid == 1)) { \
				______cpm_spad.ddr_delay.first = ______cpm_spad.ddr_delay.curr; \
				______cpm_spad.ddr_delay.first_valid = 1; \
				set_cpm_scratch_pad(______cpm_spad);	\
			}						\
		}							\
	} while (0)

#define is_ddr_spad_first_valid()					\
	({								\
		union cpm_scratch_pad ______cpm_spad = get_cpm_scratch_pad(); \
		(______cpm_spad.ddr_delay.first_valid);			\
	})

#define set_curr_valid()						\
	do{								\
		union cpm_scratch_pad ______cpm_spad = get_cpm_scratch_pad(); \
		if (______cpm_spad.ddr_delay.magic == JZ_POST_MAGIC) {	\
			______cpm_spad.ddr_delay.curr_valid = 1;	\
			set_cpm_scratch_pad(______cpm_spad);		\
		}							\
	} while(0)

#define ddr_test_can_continue()						\
	({								\
		union cpm_scratch_pad ______cpm_spad = get_cpm_scratch_pad(); \
		int ______can_continue = 1;				\
		if (______cpm_spad.ddr_delay.magic == JZ_POST_MAGIC) {	\
			______can_continue == !((______cpm_spad.ddr_delay.curr_prev_valid == 1) && \
						(______cpm_spad.ddr_delay.curr_valid == 0)); \
		}							\
									\
		(______can_continue);					\
	})

#define init_scratch_pad_ddr()						\
	do {								\
		union cpm_scratch_pad ______cpm_spad = get_cpm_scratch_pad(); \
		if (______cpm_spad.ddr_delay.magic != JZ_POST_MAGIC) {	\
			______cpm_spad.value = 0;			\
			______cpm_spad.ddr_delay.magic = JZ_POST_MAGIC; \
		} else {						\
			______cpm_spad.ddr_delay.curr_prev_valid =	\
				______cpm_spad.ddr_delay.curr_valid;	\
			______cpm_spad.ddr_delay.curr_valid = 0;	\
			______cpm_spad.ddr_delay.curr++;		\
		}							\
		set_cpm_scratch_pad(______cpm_spad);			\
	} while (0)

#ifndef	__ASSEMBLY__

struct post_test {
	char *name;
	char *cmd;
	char *desc;
	int flags;
	int (*test) (int flags);
	int (*init_f) (void);
	void (*reloc) (void);
	unsigned long testid;
};
int jz_post_init_f (void);
void jz_post_output_backlog ( void );
int post_run (char *name, int flags);
int post_info (char *name);
int post_log (char *format, ...);
void post_reloc (void);

extern struct post_test post_list[];
extern unsigned int post_list_size;

#define WATCHDOG_DISABLE()						\
	do {								\
		*(volatile unsigned int *)0xB000202c = 0x10000; /* stop WDT clock */ \
		*(volatile unsigned int *)0xB0002004 = 0x0;	/* disable WDT */ \
	} while(0)

#define WATCHDOG_RESET()						\
	do {								\
		WATCHDOG_DISABLE();					\
		*(volatile unsigned int *)0xB000200c = 0x1a;		\
		*(volatile unsigned int *)0xB0002008 = 0x0; /* CNT = 0 */ \
		*(volatile unsigned int *)0xB0002000 = 0xffff;	/* TDR */ \
		*(volatile unsigned int *)0xB000203c = 0x10000; /* start WDT clock */ \
		*(volatile unsigned int *)0xB0002004 = 0x1;	/* enable WDT */ \
	} while(0)

#define WATCHDOG_RESET2()						\
	do {								\
		WATCHDOG_DISABLE();					\
		*(volatile unsigned int *)0xB000200c = 0x1C;		\
		*(volatile unsigned int *)0xB0002008 = 0x0; /* CNT = 0 */ \
		*(volatile unsigned int *)0xB0002000 = 0xffff;	/* TDR */ \
		*(volatile unsigned int *)0xB000203c = 0x10000; /* start WDT clock */ \
		*(volatile unsigned int *)0xB0002004 = 0x1;	/* enable WDT */ \
	} while(0)

#endif /* __ASSEMBLY__ */

#define CFG_JZ_POST_CPU		0x00000001
#define CFG_JZ_POST_CACHE	0x00000002
#define CFG_JZ_POST_MEMORY	0x00000004
#define CFG_JZ_POST_RTC		0x00000008
#define CFG_JZ_POST_WATCHDOG	0x00000010

#endif /* CONFIG_POST */

#endif /* _JZ_POST_H */
