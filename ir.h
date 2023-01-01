#pragma once

#include <cstdint>
#include <list>

#include "wasmdefs.h"

using namespace std;

typedef uint8_t byte;

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
  FUNC = WASM_IE_DESC_FUNC, 
  GLOBAL = WASM_IE_DESC_GLOBAL, 
  TABLE = WASM_IE_DESC_TABLE, 
  MEMORY = WASM_IE_DESC_MEM
} wasm_kind_t;


const char* wasm_type_string(wasm_type_t type);



/* Intermediate structs */
struct {
  uint32_t initial;
  uint32_t max;
  unsigned has_max : 1;
} wasm_limits_t;


struct {
  uint32_t count;
  wasm_type_t type;
} wasm_local_decl_t;


/* Base class */
class BaseDecl {
  protected:
    static uint32_t count;
  public:
    virtual void parse_section(WasmModule &module, buffer_t *buf, uint32_t len) = 0;
};

/* Section Declarations */
class SigDecl: public BaseDecl {
  uint32_t num_params;
  wasm_type_t* params;
  uint32_t num_results;
  wasm_type_t* results;
};

class ImportDecl: public BaseDecl {
  uint32_t mod_name_length;
  const char* mod_name;
  uint32_t member_name_length;
  const char* member_name;
  wasm_kind_t kind;
  uint32_t index;
};

class FuncDecl: public BaseDecl {
  uint32_t sig_index;
  wasm_sig_decl_t* sig;
  uint32_t num_local_vec;
  uint32_t num_locals;
  wasm_local_decl_t *local_decl;
  const byte* code_start;
  const byte* code_end;
};

class TableDecl: public BaseDecl {
  wasm_limits_t limits;
};

class GlobalDecl: public BaseDecl {
  wasm_type_t type;
  unsigned mutable : 1;
  const byte* init_expr_start;
  const byte* init_expr_end;
};

class DataDecl: public BaseDecl {
  uint32_t mem_offset;
  const byte* bytes_start;
  const byte* bytes_end;
};

class ElemDecl: public BaseDecl {
  uint32_t table_offset;
  uint32_t length;
  uint32_t* func_indexes;
};

class ExportDecl: public BaseDecl {
  uint32_t length;
  char *name;
  wasm_kind_t kind;
  uint32_t index;
};

class CustomDecl: public BaseDecl {
  uint32_t name_length;
  char* name;
  uint32_t bytes_length;
  byte* bytes;
};


class WasmModule {
  list<MemoryDecl>  mems;
  list<TableDecl>   tables;
  list<SigDecl>     sigs;
  list<ImportDecl>  imports;
  list<FuncDecl>    funcs;
  list<ExportDecl>  exports;
  list<GlobalDecl>  globals;
  list<DataDecl>    datas;
  list<ElemDecl>    elems;
  list<CustomDecl>  customs;

  uint32_t start_idx;
  int has_start;
  int has_datacount;
};


