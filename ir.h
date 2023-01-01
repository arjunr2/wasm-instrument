#pragma once

#include <cstdint>
#include <cstddef>
#include <list>
#include <vector>
#include <string>
#include <algorithm>

#include "common.h"
#include "wasmdefs.h"


/* Utility Functions */
const char* wasm_type_string(wasm_type_t type);
const char* wasm_section_name(byte sec_code);

class WasmModule;

/* Intermediate structs */
struct wasm_limits_t {
  uint32_t initial;
  uint32_t max;
  unsigned has_max : 1;
};

struct wasm_local_decl_t {
  uint32_t count;
  wasm_type_t type;
};



/* Section Field Declarations */
struct CustomDecl {
  std::string name;
  bytearr bytes;
};

struct SigDecl {
  uint32_t num_params;
  wasm_type_t* params;
  uint32_t num_results;
  wasm_type_t* results;
};

struct ImportDecl {
  uint32_t mod_name_length;
  std::string  mod_name;
  uint32_t member_name_length;
  std::string member_name;
  wasm_kind_t kind;
  uint32_t index;
};

struct FuncDecl {
  uint32_t sig_index;
  SigDecl* sig;
  uint32_t num_local_vec;
  uint32_t num_locals;
  wasm_local_decl_t *local_decl;
  const byte* code_start;
  const byte* code_end;
};

struct MemoryDecl {
  wasm_limits_t limits;
};

struct TableDecl {
  wasm_limits_t limits;
};

struct GlobalDecl {
  wasm_type_t type;
  unsigned mutable : 1;
  const byte* init_expr_start;
  const byte* init_expr_end;
};

struct DataDecl {
  uint32_t mem_offset;
  const byte* bytes_start;
  const byte* bytes_end;
};

struct ElemDecl {
  uint32_t table_offset;
  uint32_t length;
  uint32_t* func_indexes;
};

struct ExportDecl {
  uint32_t length;
  std::string name;
  wasm_kind_t kind;
  uint32_t index;
};


/* Section */
struct WasmModule {
  uint32_t version;

  std::list <CustomDecl>  customs;
  std::list <SigDecl>     sigs;
  std::list <ImportDecl>  imports;
  std::list <FuncDecl>    funcs;
  std::list <TableDecl>   tables;
  std::list <MemoryDecl>  mems;
  std::list <GlobalDecl>  globals;
  std::list <ExportDecl>  exports;
  std::list <ElemDecl>    elems;
  std::list <DataDecl>    datas;

  uint32_t start_idx;
  int has_start;

  /* Only for validation */
  uint32_t num_datas;
  int has_datacount;


  #define DECODE_DECL(sec)  \
    void decode_##sec##_section (buffer_t &buf, uint32_t len);
  DECODE_DECL(type);
  DECODE_DECL(import);
  DECODE_DECL(function);
  DECODE_DECL(table);
  DECODE_DECL(memory);
  DECODE_DECL(global);
  DECODE_DECL(export);
  DECODE_DECL(start);
  DECODE_DECL(element);
  DECODE_DECL(code);
  DECODE_DECL(data);
  DECODE_DECL(datacount);
  DECODE_DECL(custom);

  #define ENCODE_DECL(sec)  \
    void encode_##sec##_section (buffer_t &buf, uint32_t len);
  ENCODE_DECL(type);
  ENCODE_DECL(import);
  ENCODE_DECL(function);
  ENCODE_DECL(table);
  ENCODE_DECL(memory);
  ENCODE_DECL(global);
  ENCODE_DECL(export);
  ENCODE_DECL(start);
  ENCODE_DECL(element);
  ENCODE_DECL(code);
  ENCODE_DECL(data);
  ENCODE_DECL(datacount);
  ENCODE_DECL(custom);
};


