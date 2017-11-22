/*-
 * Copyright (c) 2015 Martin Lucina.  All Rights Reserved.
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

#include <arch/x86/cons.h>
#include <arch/x86/hypervisor.h>

#include <bmk-core/printf.h>

char bios_com1_base;
char bios_crtc_base;

/*
 * Filled in by locore from BIOS data area.
 */
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


static int platform_puts(const char *buf, int n)
{
    struct ukvm_puts str;

    str.data = (char *)buf;
    str.len = n;

    ukvm_do_hypercall(UKVM_HYPERCALL_PUTS, &str);

    return str.len;
}


void
cons_putc(int _c)
{
	char c = _c;
	platform_puts(&c, 1);
}

void
cons_init(void)
{
	bmk_printf_init(cons_putc, NULL);
}



static unsigned long strlen(const char * str)
{
    const char *s;
    for (s = str; *s; ++s) {}
    return(s - str);
}

void
cons_puts(const char *s)
{
	platform_puts(s, strlen(s));

}
