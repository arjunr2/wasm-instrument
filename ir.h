#pragma once

#include <cstdint>
#include <cstddef>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

#include "common.h"
#include "wasmdefs.h"

class InstBase;
typedef std::shared_ptr<InstBase> InstBasePtr;
typedef std::list<InstBasePtr> InstList;

typedef std::list<wasm_type_t> typelist;

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

struct wasm_localcse_t {
  uint32_t count;
  wasm_type_t type;
};
typedef std::list<wasm_localcse_t> wasm_localcsv_t;

/* Import or export index: -1 if not imported/exported */
struct IEIdx {
  int64_t import_idx;
  int64_t export_idx;

  IEIdx(): 
    import_idx(-1), export_idx(-1) { }

  //inline void set_import_idx(int64_t i) { import_idx = i; }
  //inline void set_export_idx(int64_t i) { export_idx = i; }
};

/* Section Field Declarations */
struct CustomDecl {
  std::string name;
  bytearr bytes;
};

struct SigDecl {
  typelist params;
  typelist results;
};


struct FuncDecl: public IEIdx {
  /* From func/import section */
  SigDecl* sig;
  /* From code section */
  wasm_localcsv_t pure_locals;
  uint32_t num_pure_locals;
  bytearr code_bytes;
  /* Code: Raw static instructions */ 
  InstList instructions;
  /* CFG for function */
};

struct MemoryDecl: public IEIdx {
  wasm_limits_t limits;
};

struct TableDecl: public IEIdx {
  wasm_type_t reftype;
  wasm_limits_t limits;
};

struct GlobalDecl: public IEIdx {
  wasm_type_t type;
  unsigned is_mutable : 1;
  bytearr init_expr_bytes;
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

  inline uint32_t get_num_imports () {
    return num_funcs + num_tables + num_memories + num_globals;
  }
};


/* Section */
class WasmModule {
  private:
    uint32_t magic;
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

    /* Decode functions */
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

    /* Encode functions */
    #define ENCODE_DECL(sec)  \
      bytedeque encode_##sec##_section ();
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

    /* Code decoding for instructions */
    InstList decode_expr_to_insts (buffer_t &buf);

  public:
    /* Accessors */
    inline SigDecl* getSig(uint32_t idx)        { return GET_LIST_ELEM(this->sigs, idx); }
    inline FuncDecl* getFunc(uint32_t idx)      { return GET_LIST_ELEM(this->funcs, idx); }
    inline GlobalDecl* getGlobal(uint32_t idx)  { return GET_LIST_ELEM(this->globals, idx); }
    inline TableDecl* getTable(uint32_t idx)    { return GET_LIST_ELEM(this->tables, idx); }
    inline MemoryDecl* getMemory(uint32_t idx)  { 
      if (idx)  throw std::runtime_error("Memory Immediate must be 0\n");
      return GET_LIST_ELEM(this->mems, idx); 
    }

    /* Decode wasm file from buffer */
    void decode_buffer (buffer_t &buf);
    /* Encode module into wasm format */
    bytedeque encode_module ();
};


