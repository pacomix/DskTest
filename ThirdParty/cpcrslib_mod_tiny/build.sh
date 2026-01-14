#!/bin/bash

Z80C=$HOME/dev/amstrad/build/sdcc/darwin/4.2.0/bin/sdasz80

[ -d obj ] || mkdir obj
rm -f obj/*
$Z80C -o obj/cpcrslib_tiny_sdcc420.rel ./src/cpcrslib_tiny_sdcc420.s

Z80L=$HOME/dev/amstrad/build/sdcc/darwin/4.2.0/bin/sdar

rm -f ./lib/cpcrslibmodtiny_sdcc420.lib
$Z80L rc -o ./lib/cpcrslibmodtiny_sdcc420.lib ./obj/cpcrslib_tiny_sdcc420.rel

