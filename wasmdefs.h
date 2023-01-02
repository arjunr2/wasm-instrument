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

/* Type decoding */
#define WASM_TYPE_I32 0x7F
#define WASM_TYPE_I64 0x7E
#define WASM_TYPE_F32 0x7D
#define WASM_TYPE_F64 0x7C
#define WASM_TYPE_V128 0x7B 
#define WASM_TYPE_FUNCREF 0x70
#define WASM_TYPE_EXTERNREF 0x6F
#define WASM_TYPE_FUNC 0x60

/* Import/export desc kind decoding */
#define WASM_DESC_FUNC 0x00
#define WASM_DESC_TABLE 0x01
#define WASM_DESC_MEM 0x02
#define WASM_DESC_GLOBAL 0x03

typedef enum {
  I32 = WASM_TYPE_I32, 
  I64 = WASM_TYPE_I64,
  F32 = WASM_TYPE_F32,
  F64 = WASM_TYPE_F64, 
  V128 = WASM_TYPE_V128,
  EXTERNREF = WASM_TYPE_EXTERNREF,
  FUNCREF = WASM_TYPE_FUNCREF
} wasm_type_t;

typedef enum {
  KIND_FUNC = WASM_DESC_FUNC, 
  KIND_TABLE = WASM_DESC_TABLE, 
  KIND_MEMORY = WASM_DESC_MEM,
  KIND_GLOBAL = WASM_DESC_GLOBAL
} wasm_kind_t;


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
  IMM_VALTS
} opcode_imm_type;

/* Information associated with each opcode */
typedef struct {
  const char*       mnemonic;
  opcode_imm_type   imm_type;
  int               invalid;
} opcode_entry_t;

/* Defined in C file so we can use designated initializers */
extern opcode_entry_t opcode_table[256];

