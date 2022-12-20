#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ir.h"
#include "common.h"

wasm_value_t parse_wasm_value(char* str) {
  int len = strlen(str);
  if (len > 0) {
    if (str[len-1] == 'd' || str[len-1] == 'D') {
      // treat the input as a double
      char* end = NULL;
      double result = strtod(str, &end);
      if (end == (str + len - 1)) return wasm_f64_value(result);
    } else {
      // treat the input as an integer
      char* end = NULL;
      long result = strtol(str, &end, 10);
      if (end == (str + len)) return wasm_i32_value(result);
    }
  }
  wasm_value_t orig_string = {
    .tag = EXTERNREF,
    .val.ref = str,
  };
  return orig_string;
}

void print_wasm_value(wasm_value_t val) {
  switch (val.tag) {
  case I32:
    printf("%d", val.val.i32);
    break;
  case F64:
    printf("%lf", val.val.f64);
    break;
  case EXTERNREF:
    printf("%p", val.val.ref);
    break;
  }
}

void trace_wasm_value(wasm_value_t val) {
  switch (val.tag) {
  case I32:
    TRACE("%d", val.val.i32);
    break;
  case F64:
    TRACE("%lf", val.val.f64);
    break;
  case EXTERNREF:
    TRACE("%p", val.val.ref);
    break;
  }
}

const char* wasm_type_string(wasm_type_t type) {
  switch (type) {
    case I32: 
      return "i32"; break;
    case F64:
      return "f64"; break;
    case EXTERNREF:
      return "externref"; break;
  }
}

wasm_value_t wasm_i32_value(int32_t val) {
  wasm_value_t r = {
    .tag = I32,
    .val.i32 = val,
  };
  return r;
}

wasm_value_t wasm_f64_value(double val) {
  wasm_value_t r = {
    .tag = F64,
    .val.f64 = val,
  };
  return r;
}

wasm_value_t wasm_ref_value(void* val) {
  wasm_value_t r = {
    .tag = EXTERNREF,
    .val.ref = val,
  };
  return r;
}

inline size_t wasm_tag_size(wasm_type_t val) {
  return ((val == I32) ? 8 : 8);
}
