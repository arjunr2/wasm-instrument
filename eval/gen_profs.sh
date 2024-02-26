#!/bin/bash

for path in wasms/*.wasm; do
  file="${path##*/}"
  bench="${file%.*}"
  echo $path
  ../inspect --scheme=call-count -o profiles/$bench.call_count $path
  ../inspect --scheme=func-idx -o profiles/$bench.func_idx $path
done
