
.PHONY: data/zero
data/zero:
	rm -rf data
	mkdir -p data
	dd if=/dev/zero of=data/zero count=131072 bs=1024

lfs:
	x86_64-rumprun-netbsd-gcc test.c -o test.rump
	rumprun-bake solo5_spt test.nabla test.rump
	genlfs data data.lfs
	../solo5/tenders/spt/solo5-spt --disk=data.lfs test.nabla '{"cmdline":"/data/test.c /data/test.c","env":"a=b","blk":{"source":"etfs","path":"/dev/ld0a","fstype":"blk","mountpoint":"/data"}}'
