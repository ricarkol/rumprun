all: disk rump linux

rump: postmark-1_5.c
	x86_64-rumprun-netbsd-gcc -o postmark-rumprun postmark-1_5.c
	rumprun-bake hw-virtio postmark-rumprun.bin postmark-rumprun

linux: postmark-1_5.c
	gcc -o postmark postmark-1_5.c


.PHONY: qemu
qemu:
	rumprun qemu -b disk.img,/ -i postmark-rumprun.bin

.PHONY: kvm
kvm:
	rumprun kvm -b disk.img,/ -i postmark-rumprun.bin

.PHONY: rr
rr:
	rumprun kvm -rr -b disk.img,/ -i postmark-rumprun.bin


.PHONY: disk
disk:
	dd if=/dev/zero of=disk.img count=100000
	#mkfs.fat -S 512 disk.img
	mkfs.ext3 disk.img
	mkdir -p disk
	sudo mount disk.img disk
	sudo mkdir disk/dir
	sudo cp .pmrc disk/.
	sudo umount disk
