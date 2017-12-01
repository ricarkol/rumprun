all:
	#cp -f /home/kollerr/research/rumprun/obj-amd64-hw-solo5/dest.stage/rumprun-x86_64/lib/rumprun-hw/rumprun.o /home/kollerr/research/rumprun/./rumprun/rumprun-x86_64/lib/rumprun-hw/rumprun.o
	#cp -f /home/kollerr/research/rumprun/./obj-amd64-hw-solo5/dest.stage/rumprun-x86_64/lib/rumprun-hw/librumpnet_solo5if.a /home/kollerr/research/rumprun/./rumprun/rumprun-x86_64/lib/rumprun-hw/.
	x86_64-rumprun-netbsd-gcc -o tcp-rumprun tcp.c
	#rumprun-bake hw_solo5 tcp-rumprun.bin tcp-rumprun
	#rumprun-bake -c solo5.conf hw_solo5 tcp-rumprun.bin tcp-rumprun
	rumprun-bake hw_solo5 tcp-rumprun.bin tcp-rumprun
	gcc tcp.c -o tcp
