build:
	./build-rr.sh -j4 -d rumprun-solo5 -o ./obj solo5 build
	./build-rr.sh -j4 -d rumprun-solo5 -o ./obj solo5 install
	cp solo5_ukvm-linux-seccomp-not-dynamic_826431f.o rumprun-solo5/rumprun-x86_64/lib/rumprun-solo5/libsolo5.a

clean:
	rm -rf obj*
	rm -rf rumprun
	rm -rf rumprun-solo5*
