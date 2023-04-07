#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "views.h"
#include "instructions.h"

#include <climits>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <map>
#include <unordered_set>

#include "common.h"
#include "ir.h"

static ExportDecl* get_main_export(WasmModule &module) {
  /* look for either main or _start */
  ExportDecl* main_fn_exp = module.find_export("main");
  ExportDecl* start_fn_exp = module.find_export("_start");
  if (!(main_fn_exp || start_fn_exp)) {
    throw std::runtime_error("Could not find export \"main\" or \"_start\" function!");
  }
  return main_fn_exp ? main_fn_exp : start_fn_exp;
}

