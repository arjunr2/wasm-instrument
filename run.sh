#!/bin/bash

./instrument -o out.wasm -s $2 $1
wasm2wat --enable-threads out.wasm -o out.wat
#cmp -l b.wasm $1 | gawk '{printf "%08x %02x %02x\n", strtonum($1)-1, strtonum(0$2), strtonum(0$3)}'
