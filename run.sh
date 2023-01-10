#!/bin/bash

./instrument -o out.wasm $1
wasm2wat out.wasm -o out.wat
#cmp -l b.wasm $1 | gawk '{printf "%08x %02x %02x\n", strtonum($1)-1, strtonum(0$2), strtonum(0$3)}'
