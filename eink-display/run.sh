#!/bin/bash
make;
sudo killall test.exe;
dune build ./test.exe &&
sudo $(pwd)/_build/default/test.exe $@
