#pragma once

#include <cstdint>
#include <cstddef>
#include <list>
#include <vector>
#include <string>

#include "common.h"
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
  uint32_t name_length;
  string name;
  uint32_t bytes_length;
  byte* bytes;
};

struct SigDecl {
  uint32_t num_params;
  wasm_type_t* params;
  uint32_t num_results;
  wasm_type_t* results;
};

struct ImportDecl {
  uint32_t mod_name_length;
  string  mod_name;
  uint32_t member_name_length;
  string member_name;
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
  string name;
  wasm_kind_t kind;
  uint32_t index;
};

/* Section */
/* Base class */
template <typename T>
class Section {
  uint32_t count;
  list<T> decl;
  public:
    Section() : count(0) { }

    uint32_t get_count()        { return count; }
    void set_count(uint32_t ct) { count = ct; }

    void decode_section(WasmModule &module, buffer_t *buf, uint32_t len);
    void encode_section(buffer_t *buf, uint32_t len);

};


struct WasmModule {
  uint32_t magic;
  uint32_t version;

  uint32_t num_customs;
  list<CustomDecl>  customs;

  uint32_t num_sigs;
  list<SigDecl>     sigs;

  uint32_t num_imports;
  list<ImportDecl>  imports;

  uint32_t num_funcs;
  list<FuncDecl>    funcs;

  uint32_t num_tables;
  list<TableDecl>   tables;
  
  uint32_t num_mems;
  list<MemoryDecl>  mems;
  
  uint32_t num_globals;
  list<GlobalDecl>  globals;
  
  uint32_t num_exports;
  list<ExportDecl>  exports;

  uint32_t num_elems;
  list<ElemDecl>    elems;
  
  uint32_t num_datas;
  list<DataDecl>    datas;

  uint32_t start_idx;
  int has_start;
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


