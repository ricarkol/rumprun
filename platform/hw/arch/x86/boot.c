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

#define UKVM_HYPERCALL_PIO_BASE 0x500

#define UKVM_GUEST_PTR(T) T


/*
 * On x86, 32-bit PIO is used as the hypercall mechanism. This only supports
 * sending 32-bit pointers; raise an assertion if a bigger pointer is used.
 *
 * On x86 the compiler-only memory barrier ("memory" clobber) is sufficient
 * across the hypercall boundary.
 */
static inline void ukvm_do_hypercall(int n, volatile void *arg)
{
#    ifdef assert
    assert(((uint64_t)arg <= UINT32_MAX));
#    endif
    __asm__ __volatile__("outl %0, %1"
            :
            : "a" ((uint32_t)((uint64_t)arg)),
              "d" ((uint16_t)(UKVM_HYPERCALL_PIO_BASE + n))
            : "memory");
}

enum ukvm_hypercall {
    /* UKVM_HYPERCALL_RESERVED=0 */
    UKVM_HYPERCALL_WALLTIME=1,
    UKVM_HYPERCALL_PUTS,
    UKVM_HYPERCALL_POLL,
    UKVM_HYPERCALL_BLKINFO,
    UKVM_HYPERCALL_BLKWRITE,
    UKVM_HYPERCALL_BLKREAD,
    UKVM_HYPERCALL_NETINFO,
    UKVM_HYPERCALL_NETWRITE,
    UKVM_HYPERCALL_NETREAD,
    UKVM_HYPERCALL_HALT,
    UKVM_HYPERCALL_MAX
};


/* UKVM_HYPERCALL_PUTS */
struct ukvm_puts {
    /* IN */
    UKVM_GUEST_PTR(const char *) data;
    unsigned long len;
};




void _start(void *arg);

static int platform_puts(const char *buf, int n)
{
    struct ukvm_puts str;

    str.data = (char *)buf;
    str.len = n;

    ukvm_do_hypercall(UKVM_HYPERCALL_PUTS, &str);

    return str.len;
}

void _start(void *arg)
{
    platform_puts("hola\n", 5);

	cons_init();
    cons_putc('a');
    cons_puts("hola las pelotas\n");
	bmk_printf("rump kernel bare metal bootstrap\n\n");

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
