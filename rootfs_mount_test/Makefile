env-rumprun.seccomp: env.c
	x86_64-rumprun-netbsd-gcc -o env-rumprun env.c
	rumprun-bake solo5_ukvm_seccomp env-rumprun.seccomp env-rumprun

run_seccomp: env-rumprun.seccomp
	genisoimage -o data.iso data
	../ukvm-bin.seccomp --disk=data.iso --net=tap100 env-rumprun.seccomp -- '{"cmdline":"a","env":"a=b","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/"},"cwd":"/data"}'

run_seccomp2: env-rumprun.seccomp
	dd if=/dev/zero of=data.ext2 count=40960 bs=1024
	genext2fs -B 1024 -b 40960 -d data data.ext2
	../ukvm-bin.seccomp --disk=data.ext2 --net=tap100 env-rumprun.seccomp -- '{"cmdline":"/dev","env":"a=b","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/"},"cwd":"/data"}'

clean:
	rm -f env-rumprun.seccomp
	rm -f env-rumprun
