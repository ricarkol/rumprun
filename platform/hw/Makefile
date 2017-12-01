include ../../global.mk
include ${BUILDRUMP_TOOLFLAGS}

default: all

# Check if we're building for a supported target.
supported= false
# assume we're doing "make clean"
MACHINE?= i386
ifeq (${MACHINE},i386)
HW_MACHINE_ARCH=i386
supported:= true
endif
ifeq (${MACHINE},amd64)
supported:= true
endif
ifeq (${MACHINE},evbarm)
supported:= true
ARCHDIR= arm/integrator
endif
ifneq (${supported},true)
$(error only supported target is x86, you have ${MACHINE})
endif

ARCHDIR?= ${MACHINE}
HW_MACHINE_ARCH?= ${MACHINE_GNU_ARCH}

LDSCRIPT:=	$(abspath arch/${ARCHDIR}/kern.ldscript)
SRCS+=		intr.c clock_subr.c solo5_kernel.c multiboot.c undefs.c

INSTALLTGTS=    librumpnet_solo5if_install

include ../Makefile.inc
include arch/${ARCHDIR}/Makefile.inc

OBJS:=	$(patsubst %.c,${RROBJ}/platform/%.o,${SRCS}) \
	$(patsubst %.S,${RROBJ}/platform/%.o,${ASMS})

.PHONY:	clean cleandir all

all:  links archdirs ${MAINOBJ} ${TARGETS}

${RROBJ}/include/hw/machine:
	@mkdir -p ${RROBJ}/include/hw
	@ln -sf $(shell pwd)/include/arch/${HW_MACHINE_ARCH} $@

${RROBJ}/include/bmk-pcpu:
	@ln -sf ${RROBJ}/include/hw/machine $@

links: ${RROBJ}/include/hw/machine ${RROBJ}/include/bmk-pcpu

${RROBJ}/platform/%.o: %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o $@

${RROBJ}/platform/%.o: %.S
	${CC} -D_LOCORE ${CPPFLAGS} ${CFLAGS} -c $< -o $@

$(eval $(call BUILDLIB_target,librumpnet_solo5if,.))

solo5libs: ${RROBJLIB}/librumpnet_solo5if/librumpnet_solo5if.a

${MAINOBJ}: ${OBJS} platformlibs solo5libs
	${CC} -nostdlib ${CFLAGS} -Wl,-r ${OBJS} -o $@ \
	    -L${RROBJLIB}/libbmk_core -L${RROBJLIB}/libbmk_rumpuser \
	    -Wl,--whole-archive -lbmk_rumpuser -lbmk_core -Wl,--no-whole-archive
	${OBJCOPY} -w -G bmk_* -G rumpuser_* -G jsmn_* -G solo5_app_main \
	    -G rumprun_platform_rumpuser_init -G _start $@

clean: commonclean
	rm -f ${OBJS_BMK} include/hw/machine buildtest ${MAINOBJ}

cleandir: clean
