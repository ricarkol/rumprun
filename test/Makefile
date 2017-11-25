all:
	cp -f /home/kollerr/research/rumprun/obj-amd64-hw-solo5/dest.stage/rumprun-x86_64/lib/rumprun-hw/rumprun.o /home/kollerr/research/rumprun/./rumprun/rumprun-x86_64/lib/rumprun-hw/rumprun.o
	x86_64-rumprun-netbsd-gcc -o helloer-rumprun helloer.c
	rumprun-bake hw_virtio helloer-rumprun.bin helloer-rumprun
