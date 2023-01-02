#pragma once

#include <cstdint>
#include <cstddef>
#include <list>
#include <vector>
#include <string>
#include <algorithm>

#include "common.h"
#include "wasmdefs.h"

typedef std::list<wasm_type_t> typearr;

/* Utility Functions */
const char* wasm_type_string(wasm_type_t type);
const char* wasm_section_name(byte sec_code);
const char* wasm_kind_string(wasm_kind_t kind);

class WasmModule;

/* Intermediate components */
struct wasm_limits_t {
  uint32_t initial;
  uint32_t max;
  unsigned has_max : 1;
};


/* Section Field Declarations */
struct CustomDecl {
  std::string name;
  bytearr bytes;
};

struct SigDecl {
  typearr params;
  typearr results;
};


struct FuncDecl {
  /* From func/import section */
  SigDecl* sig;
  /* From code section */
  typearr pure_locals;
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
  unsigned is_mutable : 1;
  const byte* init_expr_start;
  const byte* init_expr_end;
};

struct DataDecl {
  uint32_t flag;
  uint32_t mem_offset;
  bytearr bytes;
};

struct ElemDecl {
  uint32_t flag;
  uint32_t table_offset;
  std::list <FuncDecl*> func_indices;
};


union Descriptor {
  FuncDecl* func;
  GlobalDecl* global;
  TableDecl* table;
  MemoryDecl* mem;
};

struct ImportDecl {
  std::string mod_name;
  std::string member_name;
  wasm_kind_t kind;
  Descriptor desc;
};

struct ExportDecl {
  std::string name;
  wasm_kind_t kind;
  Descriptor desc;
};


struct ImportInfo {
  std::list <ImportDecl> list;
  uint32_t num_funcs;
  uint32_t num_tables;
  uint32_t num_memories;
  uint32_t num_globals;
};


/* Section */
struct WasmModule {
  uint32_t version;

  std::list <CustomDecl>  customs;
  std::list <SigDecl>     sigs;
  ImportInfo              imports;
  /* Func space */
  std::list <FuncDecl>    funcs;
  /* Table space */
  std::list <TableDecl>   tables;
  /* Mem space */
  std::list <MemoryDecl>  mems;
  /* Global space */
  std::list <GlobalDecl>  globals;
  std::list <ExportDecl>  exports;
  std::list <ElemDecl>    elems;
  std::list <DataDecl>    datas;

  /* Start section */
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


