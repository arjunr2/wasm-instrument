#pragma once

#include <stdint.h>
#include "wasmdefs.h"

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


typedef struct {
  uint32_t initial;
  uint32_t max;
  unsigned has_max : 1;
} wasm_limits_t;

typedef struct {
  uint32_t mod_name_length;
  const char* mod_name;
  uint32_t member_name_length;
  const char* member_name;
  wasm_kind_t kind;
  uint32_t index;
} wasm_import_decl_t;


typedef struct {
  uint32_t count;
  wasm_type_t type;
} wasm_local_decl_t;


typedef struct {
  uint32_t num_params;
  wasm_type_t* params;
  uint32_t num_results;
  wasm_type_t* results;
} wasm_sig_decl_t;

typedef struct {
  uint32_t sig_index;
  wasm_sig_decl_t* sig;
  uint32_t num_local_vec;
  uint32_t num_locals;
  wasm_local_decl_t *local_decl;
  const byte* code_start;
  const byte* code_end;
} wasm_func_decl_t;

typedef struct {
  wasm_limits_t limits;
} wasm_table_decl_t;

typedef struct {
  wasm_type_t type;
  unsigned mutable : 1;
  const byte* init_expr_start;
  const byte* init_expr_end;
} wasm_global_decl_t;

typedef struct {
  uint32_t mem_offset;
  const byte* bytes_start;
  const byte* bytes_end;
} wasm_data_decl_t;

typedef struct {
  uint32_t table_offset;
  uint32_t length;
  uint32_t* func_indexes;
} wasm_elems_decl_t;

typedef struct {
  uint32_t length;
  char *name;
  wasm_kind_t kind;
  uint32_t index;
} wasm_export_decl_t;

typedef struct {
  uint32_t name_length;
  char* name;
  uint32_t bytes_length;
  byte* bytes;
} wasm_custom_decl_t;

typedef struct {
  uint32_t num_mems;
  wasm_limits_t mem_limits;
  
  uint32_t num_tables;
  wasm_table_decl_t* table;
  
  uint32_t num_sigs;
  wasm_sig_decl_t* sigs;

  uint32_t num_imports;
  wasm_import_decl_t* imports;

  uint32_t num_funcs;
  wasm_func_decl_t* funcs;

  uint32_t num_exports;
  wasm_export_decl_t* exports;

  uint32_t num_globals;
  wasm_global_decl_t* globals;
  
  uint32_t num_datas;
  wasm_data_decl_t* datas;

  uint32_t num_elems;
  wasm_elems_decl_t* elems;

  uint32_t num_customs;
  wasm_custom_decl_t* customs;

  int has_start;
  uint32_t start_idx;

  int has_datacount;
} wasm_module_t;


