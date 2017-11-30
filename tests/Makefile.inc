include ${RUMPRUN_MKCONF}

CC= x86_64-rumprun-netbsd-gcc
CXX= x86_64-rumprun-netbsd-g++

CFLAGS+= -Wall -Werror -Wmissing-prototypes -Wstrict-prototypes -g
LDLIBS= -lrumprun_tester
RUMPBAKE_PLATFORM=hw_solo5
RUMPRUN_BAKE=rumprun-bake -c ../../test/solo5.conf

%:: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

%:: %.cc
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

%.bin: %
	$(RUMPRUN_BAKE) $(RUMPBAKE_PLATFORM) $@ $<
