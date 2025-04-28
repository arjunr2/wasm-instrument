#!/bin/bash

find . -iname '*.cpp' -o -iname '*.hpp' | xargs clang-format-19 -i
