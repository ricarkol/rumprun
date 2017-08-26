all: disk rump

rump: postmark-1_5.c
	x86_64-rumprun-netbsd-gcc -o helloer-rumprun postmark-1_5.c
	rumprun-bake hw-virtio helloer-rumprun.bin helloer-rumprun

.PHONY: run
run:
	rumprun qemu -b disk.img,/ -i helloer-rumprun.bin

.PHONY: disk
disk:
	dd if=/dev/zero of=disk.img count=100000
	#mkfs.fat -S 512 disk.img
	mkfs.ext3 disk.img
