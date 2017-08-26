all: unix sqlite_test

unix: tcph.c sqlite3.c
	gcc tcph.c sqlite3.c -ldl -o tcph

rump: tcph.c sqlite3.c
	x86_64-rumprun-netbsd-gcc -o tcph-rumprun tcph.c sqlite3.c
	rumprun-bake hw-virtio tcph-rumprun.bin tcph-rumprun

.PHONY: qemu
qemu:
	rumprun qemu -b my.disk,/ -i tcph-rumprun.bin

.PHONY: kvm
kvm:
	rumprun kvm -b my.disk,/ -i tcph-rumprun.bin

.PHONY: rr
rr:
	rumprun qemu -b my.disk,/ -r -i tcph-rumprun.bin

sqlite_test: sqlite_test.c sqlite3.c
	gcc sqlite_test.c sqlite3.c -ldl -o sqlite_test

clean:
	rm -f tcph
