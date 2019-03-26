#!/bin/sh
killall test.exe;
dune build server/test.exe &&
sudo ~/IT8951/_build/default/server/test.exe 
