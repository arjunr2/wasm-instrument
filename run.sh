#!/bin/bash

./instrument -o out.wasm -s $2 $1
wasm2wat --enable-threads out.wasm -o out.wat
wasm2wat --enable-threads $1 -o ref.wat
diff  out.wat ref.wat
