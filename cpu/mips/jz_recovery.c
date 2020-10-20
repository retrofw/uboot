/*
 * JzRISC Recovry Support
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
#include <config.h>
#include <asm/mipsregs.h>

#include <common.h>
#include <command.h>
#if defined(CONFIG_JZ4760)
#include <asm/jz4760.h>
#elif defined(CONFIG_JZ4760B)
#include <asm/jz4760b.h>
#elif defined(CONFIG_JZ4750D)
#include <asm/jz4750d.h>
#elif defined(CONFIG_JZ4750)
#include <asm/jz4750.h>
#endif
#include <linux/stddef.h>
#include <malloc.h>

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#define dputs(x...)   puts(x)
#else
#define dputs(x...)
#endif


#ifdef CFG_JZ_LINUX_RECOVERY

unsigned int is_jz_linux_recovery;
extern unsigned char default_environment[];
extern unsigned char normal_environment[];
extern unsigned char recovery_environment[];

#define BOOT_NORMAL   			0
#define BOOT_RECOVERY_KEY   	1
#define BOOT_RECOVERY_REG  		2
#define BOOT_RECOVERY_MISC   	3
#define BOOT_DEFAULT			BOOT_NORMAL

#ifdef CFG_SUPPORT_RECOVERY_MISC
#include "boot_msg.h"

#define UPDATE_UBOOT			" update uboot "
#define UPDATE_RADIO			" update radio "
/*
 * Bootloader message (stored in MISC partition)
 */
struct bootloader_message g_boot_msg;

/*
 * Handle the command that specified in bootloader_message and return boot select.
 *
 * Command:	"update firmware";
 *		"boot into recovery";
 *		any other commands we have already defined.
 *
 * Ret:	1	Normal boot
 *	2	Recovery boot
 */

int update_firmware(const char* fw_select)
{
	puts("Sorry,UPDATE_RADIO and UPDATE_UBOOT isn't supported in u-boot currently!\n");
	puts("You could do it in recovery system!\n");
	return 0;
}
static int handle_bootloader_command(void)
{
	/* Command: boot-recovery */
	if ( !memcmp(g_boot_msg.command, "boot-recovery", strlen("boot-recovery")) ) {
		puts("In handle_bootloader_command ... boot-recovery ...\n");
		return BOOT_RECOVERY_MISC;
	}

	/* Command: update-radio */
	if ( !memcmp(g_boot_msg.command, "update-radio", strlen("update-radio")) ) {
		puts("In handle_bootloader_command ... update-radio ...\n");
		update_firmware(UPDATE_RADIO);
		return BOOT_RECOVERY_MISC;
	}

	/* Command: update-uboot */
	if ( !memcmp(g_boot_msg.command, "update-uboot", strlen("update-uboot")) ) {
		puts("In handle_bootloader_command ... update-xboot ...\n");
		update_firmware(UPDATE_UBOOT);
		return BOOT_RECOVERY_MISC;
	}

	if (g_boot_msg.command[0] != '\0') {
		puts("WARNING: bootloader_message -> command [0] is not '\\0' !\n");
	}

	dputs("In handle_bootloader_command ... default ...\n");

	//dump_ram(&g_boot_msg, sizeof(struct bootloader_message));

	return BOOT_NORMAL;
}
void clear_bootloader_message_command()
{
	memset(g_boot_msg.command, '\0', sizeof(g_boot_msg.command));
	set_bootloader_message(&g_boot_msg);
}
void clear_bootloader_message()
{
	memset(&g_boot_msg, '\0', sizeof(g_boot_msg));
	set_bootloader_message(&g_boot_msg);
}
#endif

#ifdef CFG_SUPPORT_RECOVERY_KEY
int is_recovery_keys_pressed(void)
{
	int key_1, key_2, key_3;

	__gpio_set_pin(UBOOT_SEL_REVY_KEY1);
	__gpio_set_pin(UBOOT_SEL_REVY_KEY2);
	__gpio_set_pin(UBOOT_SEL_REVY_KEY3);

	__gpio_as_input(UBOOT_SEL_REVY_KEY1);
	__gpio_as_input(UBOOT_SEL_REVY_KEY2);
	__gpio_as_input(UBOOT_SEL_REVY_KEY3);

	key_1 = __gpio_get_pin(UBOOT_SEL_REVY_KEY1);
	key_2 = __gpio_get_pin(UBOOT_SEL_REVY_KEY1);
	key_3 = __gpio_get_pin(UBOOT_SEL_REVY_KEY1);

	return !(key_1 || key_2 || key_3);
}
#endif

/*
 * Get recovery signature and reset it.
 */
#ifdef CFG_SUPPORT_RECOVERY_REG
static int get_recovery_signature(void)
{
	if (__cpm_get_scrpad() == RECOVERY_SIGNATURE) {
		/* Retset the signature ,reset the signature to force into normal boot after factory reset*/
		__cpm_set_scrpad(RECOVERY_SIGNATURE_SEC);
		return 1;
	} else {
		return 0;
	}
}
#endif

int recovery_mode_check(void)
{
#ifdef CFG_SUPPORT_RECOVERY_MISC
	/* Handle boot message (MISC partition). */
	memset(&g_boot_msg, '\0', sizeof(struct bootloader_message));
	if (get_bootloader_message(&g_boot_msg) ) {
		puts("Got bootloader message failed !\n");
	} else {
		if(handle_bootloader_command()==BOOT_RECOVERY_MISC)
            return BOOT_RECOVERY_MISC;
	}
#endif
#ifdef CFG_SUPPORT_RECOVERY_KEY
	/* Recovery boot keys */
	if (is_recovery_keys_pressed()){
		return BOOT_RECOVERY_KEY;
	}
#endif
#ifdef CFG_SUPPORT_RECOVERY_REG
	/* Recovery signature */
	if(get_recovery_signature()){
		return BOOT_RECOVERY_REG;
	}
#endif
	return BOOT_NORMAL;
}
void jz_recovery_handle(void)
{
	unsigned int signature = 0;

	is_jz_linux_recovery=recovery_mode_check();
	puts ("\n");
	if(is_jz_linux_recovery == BOOT_RECOVERY_KEY)
		puts ("RECOVERY(Key) Booting ...\n");
	else if(is_jz_linux_recovery == BOOT_RECOVERY_REG)
		puts ("RECOVERY(Reg) Booting ...\n");
	else if(is_jz_linux_recovery == BOOT_RECOVERY_MISC){
		puts ("RECOVERY(MISC) Booting ...\n");
	}else
		puts ("NORMAL Booting ...\n");
	puts ("\n");

#ifdef CFG_SUPPORT_RECOVERY_REG
	signature = __cpm_get_scrpad();
	if ((signature == RECOVERY_SIGNATURE) || (signature == RECOVERY_SIGNATURE_SEC)) {
		if (signature == RECOVERY_SIGNATURE_SEC)
			__cpm_set_scrpad(0);
	}
#endif
#ifdef CFG_SUPPORT_RECOVERY_MISC
	dputs("clear_bootloader_message ...\n");
	clear_bootloader_message_command();
#endif

	memset(default_environment,0,CONFIG_DEFAULT_ENV_SIZE);
	if(is_jz_linux_recovery != BOOT_NORMAL)
		memcpy(default_environment,recovery_environment,CONFIG_DEFAULT_ENV_SIZE);
	else
		memcpy(default_environment,normal_environment,CONFIG_DEFAULT_ENV_SIZE);


#if 0
	DEBUGF("CFG_NAND_BLOCK_SIZE:0x%lx,CFG_NAND_U_BOOT_SIZE=0x%lx\n",CFG_NAND_BLOCK_SIZE,CFG_NAND_U_BOOT_SIZE);
	DEBUGF("CFG_ENV_OFFSET=0x%lx,CFG_ENV_OFFSET_REDUND=0x%lx\n",CFG_ENV_OFFSET,CFG_ENV_OFFSET_REDUND);
	DEBUGF("CFG_ENV_REVY_OFFSET=0x%lx,CFG_ENV_REVY_OFFSET_REDUND=0x%lx\n",CFG_ENV_REVY_OFFSET,CFG_ENV_REVY_OFFSET_REDUND);
#endif
}

#endif  /* CFG_JZ_LINUX_RECOVERY */

