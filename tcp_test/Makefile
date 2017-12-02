all:
	x86_64-rumprun-netbsd-gcc -o tcp-rumprun tcp.c
	rumprun-bake hw_solo5 tcp-rumprun.bin tcp-rumprun
	gcc tcp.c -o tcp
