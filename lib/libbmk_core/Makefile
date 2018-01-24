LIB=		bmk_core
LIBISPRIVATE=	# defined

SRCS=		init.c bmk_string.c jsmn.c memalloc.c pgalloc.c sched.c
SRCS+=		subr_prf.c strtoul.c

# kernel-level source code
CFLAGS+=	-fno-stack-protector -fPIC
AFLAGS+=	-fno-stack-protector -fpic -fPIC

CPPFLAGS+=	-I${.CURDIR}/../../include -fPIC

.include "${.CURDIR}/arch/${MACHINE_ARCH}/Makefile.inc"

.include <bsd.lib.mk>
