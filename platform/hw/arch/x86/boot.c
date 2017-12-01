/*-
 * Copyright (c) 2014 Antti Kantee.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <hw/types.h>
#include <hw/kernel.h>
#include <hw/multiboot.h>

#include <bmk-core/core.h>
#include <bmk-core/mainthread.h>
#include <bmk-core/sched.h>
#include <bmk-core/printf.h>

#include <bmk-core/solo5.h>
#include <bmk-core/pgalloc.h>

#include <net/if.h>

#define HEAP_SIZE	(256e6) // 256 MBs hardcoded XXX

extern int if_virt_solo5if_modcmd(int cmd, void *opaque);

int solo5_app_main(char *cmdline);

int solo5_app_main(char *cmdline)
{
	unsigned long heap;

	cons_init();
	bmk_printf("rump kernel bare metal bootstrap\n\n");

	bmk_sched_init();

	heap = (unsigned long) solo5_calloc(HEAP_SIZE, 1);
	bmk_assert(heap != 0);

	bmk_core_init(BMK_THREAD_STACK_PAGE_ORDER);

	bmk_pgalloc_loadmem(heap, heap + HEAP_SIZE);
        bmk_memsize = HEAP_SIZE;

	spl0();

	//extern struct if_clone VIF_CLONER; /* XXX */

	//if_clone_attach(&VIF_CLONER);

	//if_clone_attach(&solo5_vif_cloner);
	//if_virt_solo5if_modcmd(MODULE_CMD_INIT, NULL);
	//if_virt_solo5if_modcmd(0, NULL);

	bmk_sched_startmain(bmk_mainthread, cmdline);
	bmk_printf("done with bmk_sched_startmain\n\n");

	__asm__ __volatile__("cli; hlt");
	while (1);
}

void
x86_boot(struct multiboot_info *mbi)
{

	cons_init();
	bmk_printf("rump kernel bare metal bootstrap\n\n");

	cpu_init();
	bmk_sched_init();
	multiboot(mbi);

	spl0();

	bmk_sched_startmain(bmk_mainthread, multiboot_cmdline);
}
