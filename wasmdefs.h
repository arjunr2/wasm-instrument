#pragma once

#include "wasmops.h"

#define WASM_MAGIC 0x6d736100u
#define WASM_VERSION 1

#define PAGE_SIZE 65536

/* Section constants */
typedef enum {
  WASM_SECT_CUSTOM = 0,
  WASM_SECT_TYPE,
  WASM_SECT_IMPORT,
  WASM_SECT_FUNCTION,
  WASM_SECT_TABLE,
  WASM_SECT_MEMORY,
  WASM_SECT_GLOBAL,
  WASM_SECT_EXPORT,
  WASM_SECT_START,
  WASM_SECT_ELEMENT,
  WASM_SECT_CODE,
  WASM_SECT_DATA,
  WASM_SECT_DATACOUNT
} wasm_section_t;


/* Import/export desc kind decoding */
#define WASM_DESC_FUNC 0x00
#define WASM_DESC_TABLE 0x01
#define WASM_DESC_MEM 0x02
#define WASM_DESC_GLOBAL 0x03

/* Type decoding */
typedef enum {
  WASM_TYPE_I32 = 0x7F, 
  WASM_TYPE_I64 = 0x7E,
  WASM_TYPE_F32 = 0x7D,
  WASM_TYPE_F64 = 0x7C, 
  WASM_TYPE_V128 = 0x7B,
  WASM_TYPE_FUNCREF = 0x70,
  WASM_TYPE_EXTERNREF = 0x6F,
  WASM_TYPE_FUNC = 0x60
} wasm_type_t;

/* Kind decoding */
typedef enum {
  KIND_FUNC = WASM_DESC_FUNC, 
  KIND_TABLE = WASM_DESC_TABLE, 
  KIND_MEMORY = WASM_DESC_MEM,
  KIND_GLOBAL = WASM_DESC_GLOBAL
} wasm_kind_t;

static inline bool isReftype(wasm_type_t type) { 
  return (type == WASM_TYPE_EXTERNREF) || (type == WASM_TYPE_FUNCREF);
}

/* Other common defines/expressions */
#define INIT_EXPR(type, val) ({  \
  bytearr { WASM_OP_##type, val, WASM_OP_END }; \
})
