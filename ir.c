#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ir.h"
#include "common.h"

const char* wasm_type_string(wasm_type_t type) {
  switch (type) {
    case I32:       return "i32"; break;
    case I64:       return "i64"; break;
    case F32:       return "f32"; break;
    case F64:       return "f64"; break;
    case V128:      return "v128"; break;
    case EXTERNREF: return "externref"; break;
    case FUNCREF:   return "funcref"; break;
  }
  return NULL;
}

