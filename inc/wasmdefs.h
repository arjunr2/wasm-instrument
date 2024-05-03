#pragma once

#include "wasmops.h"
#include <stdbool.h>
#include <stdint.h>

#define WASM_MAGIC 0x6d736100u
#define WASM_VERSION 1

#define WASM_PAGE_SIZE 65536

/* Other common defines/expressions */
#define INIT_EXPR(type, val) ({  \
  bytedeque bdeq; \
  int ty = WASM_OP_##type;  \
  bdeq.push_back(ty); \
  switch (ty) { \
    case WASM_OP_I32_CONST: WR_I32(val); break; \
    case WASM_OP_I64_CONST: WR_I64(val); break;  \
    case WASM_OP_F32_CONST: WR_U32_RAW(val); break;  \
    case WASM_OP_F64_CONST: WR_U64_RAW(val); break; \
    default: throw std::runtime_error("Invalid initializer expression type\n"); \
  } \
  bdeq.push_back(WASM_OP_END);  \
  bytearr (bdeq.begin(), bdeq.end()); \
})

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

/* Common types */
typedef union V128_t {
  uint64_t v[2];
} V128_t;

typedef uint32_t Opcode_t;
typedef uint8_t Laneidx_t;
typedef struct {
  Laneidx_t l[16];
} Laneidx16_t;

static inline bool isReftype(wasm_type_t type) { 
  return (type == WASM_TYPE_EXTERNREF) || (type == WASM_TYPE_FUNCREF);
}


/* Opcode immediate types */
typedef enum {
  IMM_NONE = 0,
  IMM_BLOCKT,
  IMM_LABEL,
  IMM_LABELS,
  IMM_FUNC,
  IMM_SIG_TABLE,
  IMM_LOCAL,
  IMM_GLOBAL,
  IMM_TABLE,
  IMM_MEMARG,
  IMM_I32,
  IMM_F64,
  IMM_MEMORY,
  IMM_TAG,
  IMM_I64,
  IMM_F32,
  IMM_REFNULLT,
  IMM_VALTS,
  // Extension immediates
  IMM_DATA_MEMORY,
  IMM_DATA,
  IMM_MEMORYCP,
  IMM_DATA_TABLE,
  IMM_TABLECP,
  // SIMD
  IMM_V128,
  IMM_LANEIDX,
  IMM_LANEIDX16,
  IMM_MEMARG_LANEIDX
} opcode_imm_type;

/* Information associated with each opcode */
typedef struct {
  const char*       mnemonic;
  opcode_imm_type   imm_type;
  int               invalid;
} opcode_entry_t;

/* Defined in C file so we can use designated initializers */
extern opcode_entry_t opcode_table[];
