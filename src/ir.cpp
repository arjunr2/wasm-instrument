#include "ir.h"

const char* wasm_type_string(wasm_type_t type) {
  switch (type) {
    case WASM_TYPE_I32:       return "i32"; break;
    case WASM_TYPE_I64:       return "i64"; break;
    case WASM_TYPE_F32:       return "f32"; break;
    case WASM_TYPE_F64:       return "f64"; break;
    case WASM_TYPE_V128:      return "v128"; break;
    case WASM_TYPE_EXTERNREF: return "externref"; break;
    case WASM_TYPE_FUNCREF:   return "funcref"; break;
    default:  
      return "unknown";
  }
}

const char* wasm_kind_string(wasm_kind_t kind) {
  switch (kind) {
    case KIND_FUNC:     return "func"; break;
    case KIND_TABLE:    return "table"; break;
    case KIND_MEMORY:   return "memory"; break;
    case KIND_GLOBAL:   return "global"; break;
    default:  
      return "unknown";
  }
}

// Returns the string name of a section code.
const char* wasm_section_name(byte code) {
  switch (code) {
  case WASM_SECT_TYPE: return "type";
  case WASM_SECT_IMPORT: return "import";
  case WASM_SECT_FUNCTION: return "function";
  case WASM_SECT_TABLE: return "table";
  case WASM_SECT_MEMORY: return "memory";
  case WASM_SECT_GLOBAL: return "global";
  case WASM_SECT_EXPORT: return "export";
  case WASM_SECT_START: return "start";
  case WASM_SECT_ELEMENT: return "element";
  case WASM_SECT_CODE: return "code";
  case WASM_SECT_DATA: return "data";
  case WASM_SECT_DATACOUNT: return "datacount";
  case WASM_SECT_CUSTOM: return "custom";
  default:
    return "unknown";
  }
}

