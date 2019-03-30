#!/bin/sh
make;
sudo killall test.exe;
dune build frontend/test.exe &&
sudo ~/IT8951/_build/default/frontend/test.exe $@
