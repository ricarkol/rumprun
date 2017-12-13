#!/bin/bash

./ukvm-bin --net=tap100 tcp-rumprun.bin '{"cmdline":"","net":{"if":"ukvmif0","cloner":"True","type":"inet","method":"static","addr":"10.0.0.2","mask":"16"}}' 
