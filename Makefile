build:
	CC=gcc CXX=g++ ./build-rr.sh -j4 -d rumprun-solo5 -o ./obj solo5 build
	CC=gcc CXX=g++ ./build-rr.sh -j4 -d rumprun-solo5 -o ./obj solo5 install

build_hw:
	CC=gcc ./build-rr.sh -j4 -d rumprun-solo5 -o ./obj hw

clean:
	rm -rf obj*
	rm -rf rumprun
	rm -rf rumprun-solo5*
