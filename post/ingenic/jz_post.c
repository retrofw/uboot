#include <common.h>
#include <console.h>
#include <jz_post.h>

#ifdef CONFIG_POST

DECLARE_GLOBAL_DATA_PTR;

#define POST_MAX_NUMBER		32

int post_init_f (void)
{
	int res = 0;
	unsigned int i;

	for (i = 0; i < post_list_size; i++) {
		struct post_test *test = post_list + i;

		if (test->init_f && test->init_f()) {
			res = -1;
		}
	}

	return res;
}

#if 0
static void post_get_flags (int *test_flags)
{
	int  flag[] = {  POST_POWERON,   POST_NORMAL,   POST_SLOWTEST };
	char *var[] = { "post_poweron", "post_normal", "post_slowtest" };
	int varnum = sizeof (var) / sizeof (var[0]);
	char list[128];			/* long enough for POST list */
	char *name;
	char *s;
	int last;
	int i, j;

	for (j = 0; j < post_list_size; j++) {
		test_flags[j] = post_list[j].flags;
	}

	for (i = 0; i < varnum; i++) {
		if (getenv_r (var[i], list, sizeof (list)) <= 0)
			continue;

		for (j = 0; j < post_list_size; j++) {
			test_flags[j] &= ~flag[i];
		}

		last = 0;
		name = list;
		while (!last) {
			while (*name && *name == ' ')
				name++;
			if (*name == 0)
				break;
			s = name + 1;
			while (*s && *s != ' ')
				s++;
			if (*s == 0)
				last = 1;
			else
				*s = 0;

			for (j = 0; j < post_list_size; j++) {
				if (strcmp (post_list[j].cmd, name) == 0) {
					test_flags[j] |= flag[i];
					break;
				}
			}

			if (j == post_list_size) {
				printf ("No such test: %s\n", name);
			}

			name = s + 1;
		}
	}

	for (j = 0; j < post_list_size; j++) {
		if (test_flags[j] & POST_POWERON) {
			test_flags[j] |= POST_SLOWTEST;
		}
	}
}
#endif

static int post_run_single (struct post_test *test)
{
	WATCHDOG_RESET();
	post_log ("POST %s \n", test->cmd);


	if ((*test->test) (0) != 0) {
		post_log ("FAILED\n");
	} else {
		post_log ("PASSED\n");
	}
}

int post_run (char *name, int flags)
{
	unsigned int i;

	/* run all test */
	if (name == NULL) {
		for (i = 0; i < post_list_size; i++)
			post_run_single (post_list + i);

		return 0;
	} else {
		for (i = 0; i < post_list_size; i++) {
			if (strcmp (post_list[i].cmd, name) == 0)
				break;
		}

		if (i < post_list_size) {
			return post_run_single (post_list + i);
		} else {
			return -1;
		}
	}
}

static int post_info_single (struct post_test *test, int full)
{
	if (full)
		printf ("%s - %s\n"
			"  %s\n", test->cmd, test->name, test->desc);
	else
		printf ("  %-15s - %s\n", test->cmd, test->name);

	return 0;
}

/* called by cmd_diag */
int post_info (char *name)
{
	unsigned int i;

	if (name == NULL) {
		for (i = 0; i < post_list_size; i++) {
			post_info_single (post_list + i, 0);
		}

		return 0;
	} else {
		for (i = 0; i < post_list_size; i++) {
			if (strcmp (post_list[i].cmd, name) == 0)
				break;
		}

		if (i < post_list_size) {
			return post_info_single (post_list + i, 1);
		} else {
			return -1;
		}
	}
}

int post_log (char *format, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, format);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, format, args);
	va_end (args);
	/* Send to the stdout file */
	puts (printbuffer);

	return 0;
}

void post_reloc (void)
{
	unsigned int i;

	/*
	 * We have to relocate the test table manually
	 */
	for (i = 0; i < post_list_size; i++) {
		ulong addr;
		struct post_test *test = post_list + i;

		if (test->name) {
			addr = (ulong) (test->name) + gd->reloc_off;
			test->name = (char *) addr;
		}

		if (test->cmd) {
			addr = (ulong) (test->cmd) + gd->reloc_off;
			test->cmd = (char *) addr;
		}

		if (test->desc) {
			addr = (ulong) (test->desc) + gd->reloc_off;
			test->desc = (char *) addr;
		}

		if (test->test) {
			addr = (ulong) (test->test) + gd->reloc_off;
			test->test = (int (*)(int flags)) addr;
		}

		if (test->init_f) {
			addr = (ulong) (test->init_f) + gd->reloc_off;
			test->init_f = (int (*)(void)) addr;
		}

		if (test->reloc) {
			addr = (ulong) (test->reloc) + gd->reloc_off;
			test->reloc = (void (*)(void)) addr;

			test->reloc();
		}
	}
}

#endif /* CONFIG_POST */
