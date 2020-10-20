#include <common.h>

#ifdef CONFIG_POST

#include <jz_post.h>

extern int cache_post_test (int flags);
extern int watchdog_post_test (int flags);
extern int rtc_post_test (int flags);
extern int memory_post_test (int flags);
extern int cpu_post_test (int flags);

struct post_test post_list[] =
	{
#if CONFIG_POST & CFG_JZ_POST_CPU
		{
			"CPU test",
			"cpu",
			"This test verifies the arithmetic logic unit of CPU.",
			0,
			&cpu_post_test,
			NULL,
			NULL,
			0
		},
#endif
#if CONFIG_POST & CFG_JZ_POST_CACHE
		{
			"Cache test",
			"cache",
			"This test verifies the CPU cache operation.",
			0,
			&cache_post_test,
			NULL,
			NULL,
			1
		},
#endif
#if CONFIG_POST & CFG_JZ_POST_MEMORY
		{
			"Memory test",
			"memory",
			"This test checks RAM.",
			0,
			&memory_post_test,
			NULL,
			NULL,
			2
		},
#endif
#if CONFIG_POST & CFG_JZ_POST_RTC
		{
			"RTC test",
			"rtc",
			"This test verifies the RTC operation.",
			0,
			&rtc_post_test,
			NULL,
			NULL,
			3
		},
#endif
#if CONFIG_POST & CFG_JZ_POST_WATCHDOG
		{
			"Watchdog timer test",
			"watchdog",
			"This test checks the watchdog timer.",
			0,
			&watchdog_post_test,
			NULL,
			NULL,
			4
		},
#endif
	};

unsigned int post_list_size = sizeof (post_list) / sizeof (struct post_test);

#endif /* CONFIG_POST */
