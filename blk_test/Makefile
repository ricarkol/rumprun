.PHONY: setup

blk-rumprun.ukvm: blk.c
	x86_64-rumprun-netbsd-gcc -o blk-rumprun blk.c
	rumprun-bake solo5_ukvm_net blk-rumprun.ukvm blk-rumprun

blk-rumprun.seccomp: blk.c
	cp ../../solo5/kernel/ukvm/solo5.o ../rumprun-solo5/rumprun-x86_64/lib/rumprun-solo5/libsolo5_seccomp.a
	x86_64-rumprun-netbsd-gcc -o blk-rumprun blk.c
	rumprun-bake solo5_ukvm_seccomp blk-rumprun.seccomp blk-rumprun

.PHONY: ukvm_rr
ukvm_rr:
	x86_64-rumprun-netbsd-gcc -o blk-rumprun blk.c
	rumprun-bake solo5_ukvm_rr_net blk-rumprun.bin blk-rumprun

.PHONY: hw
hw:
	x86_64-rumprun-netbsd-gcc -o blk-rumprun blk.c
	rumprun-bake hw_virtio blk-rumprun.bin blk-rumprun

.PHONY: run_hw
run_hw: hw
	rumprun kvm -g '-nographic -vga none' -i \
		-I if,vioif,"-net tap,script=no,ifname=tap100" \
		-W if,inet,static,10.0.0.2/24 \
		-- blk-rumprun.bin
test.ext2:
	dd if=/dev/zero of=test.ext2 count=1000 bs=1024
	genext2fs -B 1024 -b 1000 -d test test.ext2
	#mkfs.ext2 test.ext2
	#e2cp test/aaaaaaaaaaaaaaax test.ext2:/.

test.lfs:
	dd if=/dev/zero of=test.lfs count=1000 bs=1024
	genext2fs -B 1024 -b 1000 -d test test.lfs

test.zfs: FORCE
	rm -f test.zfs
	sudo zpool destroy -f nabla || true
	dd if=/dev/zero of=test.zfs count=100 bs=1024000
	sudo zpool create -o version=23 nabla /home/kollerr/nabla-base-build/rumprun/blk_test/test.zfs
	#sudo zpool create -o version=23 -m legacy nabla /home/kollerr/nabla-base-build/rumprun/blk_test/test.zfs
	#sudo zpool create -o version=23 -O version=5 -m legacy nabla /home/kollerr/nabla-base-build/rumprun/blk_test/test.zfs
	#sudo zpool create -O version=5 -m legacy nabla /home/kollerr/nabla-base-build/rumprun/blk_test/test.zfs
	#sudo zpool upgrade -V 23 nabla4
	sudo chown kollerr:kollerr test.zfs

test.nilfs: FORCE
	rm -f test.nilfs
	dd if=/dev/zero of=test.nilfs count=200000 bs=1024
	mkfs.nilfs2 test.nilfs
	mkdir -p /tmp/nilfs
	sudo mount test.nilfs /tmp/nilfs
	sudo cp -r test /tmp/nilfs/.
	sudo umount test.nilfs
	chown kollerr:kollerr test.nilfs

FORCE:

test.iso: test
	genisoimage -U -J -f -joliet-long -r -allow-lowercase -allow-multidot -o test.iso test

.PHONY: run_ukvm
run_ukvm: blk-rumprun.ukvm test.iso test.ext2
	touch dummy
	#../ukvm-bin.vm --disk=test.iso --net=tap100 blk-rumprun.ukvm '{"cmdline":"","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/test"}}'
	../ukvm-bin.vm --disk=test.ext2 --net=tap100 blk-rumprun.ukvm '{"cmdline":"","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/test"}}'
	e2tail test.ext2:/bla

run_seccomp_ext2: blk-rumprun.seccomp test.iso test.ext2
	touch dummy
	#../ukvm-bin.seccomp --disk=test.iso --net=tap100 blk-rumprun.seccomp '{"cmdline":"","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/test"}}'
	#../ukvm-bin.seccomp --disk=test.ext2 --net=tap100 blk-rumprun.seccomp '{"cmdline":"","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/test"}}'
	../ukvm-bin.seccomp --disk=test.ext2 --net=tap100 blk-rumprun.seccomp '{"cmdline":"","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/test"}}'
	e2tail test.ext2:/bla

run_seccomp_lfs: blk-rumprun.seccomp
	../ukvm-bin.seccomp --disk=test.lfs --net=tap100 blk-rumprun.seccomp '{"cmdline":"","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/test"}}'

run_seccomp_nilfs: blk-rumprun.seccomp test.nilfs
	../ukvm-bin.seccomp --disk=test.nilfs --net=tap100 blk-rumprun.seccomp '{"cmdline":"","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/test"}}'

run_seccomp: blk-rumprun.seccomp
	../ukvm-bin.seccomp --disk=test.zfs --net=tap100 blk-rumprun.seccomp '{"cmdline":"","blk":{"source":"etfs","path":"ld0a","fstype":"blk","mountpoint":"/test"}}'

run_gdb: blk-rumprun.seccomp
	cgdb --ex="b ukvm_elf_load" --ex="r --disk=test.lfs --net=tap100 blk-rumprun.seccomp '{\"cmdline\":\"\",\"blk\":{\"source\":\"etfs\",\"path\":\"/dev/ld0a\",\"fstype\":\"blk\",\"mountpoint\":\"/test\"}}'" --ex="finish" --ex="add-symbol-file blk-rumprun.seccomp 0x100000" ../ukvm-bin.seccomp
	#cgdb --ex="b ukvm_elf_load" --ex="r --disk=test.ext2 --net=tap100 blk-rumprun.seccomp '{\"cmdline\":\"\",\"blk\":{\"source\":\"etfs\",\"path\":\"/dev/ld0a\",\"fstype\":\"blk\",\"mountpoint\":\"/test\"}}'" --ex="finish" --ex="add-symbol-file blk-rumprun.seccomp 0x100000" ../ukvm-bin.seccomp

.PHONY: run_ukvm_rr
run_ukvm_rr: ukvm_rr
	touch dummy
	../ukvm-bin.rr --disk=dummy --net=tap100 blk-rumprun.bin '{"cmdline":"","net":{"if":"ukvmif0","cloner":"True","type":"inet","method":"static","addr":"10.0.0.2","mask":"16"}}'

.PHONY: clean
clean:
	rm -f blk-rumprun.bin blk-rumprun dummy blk-rumprun.ukvm blk-rumprun.seccomp test.iso test.ext2
