#!/bin/sh
make;
sudo killall test.exe;
dune build test/test.exe &&
sudo ~/IT8951/_build/default/test/test.exe $@
