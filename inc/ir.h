#pragma once

#include <sys/syscall.h>
#include <cstdint>
#include <cstddef>
#include <list>
#include <string>
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "common.h"
#include "wasmdefs.h"


/* For instruction parsing */
class InstBase;
typedef std::shared_ptr<InstBase> InstBasePtr;
typedef std::list<InstBasePtr> InstList;
typedef std::list<InstBasePtr>::iterator InstItr;

typedef std::list<wasm_type_t> typelist;

struct ScopeBlock;
typedef std::list<ScopeBlock> ScopeList;

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
  unsigned flag : 1;
};

struct wasm_localcse_t {
  uint32_t count;
  wasm_type_t type;
};
typedef std::list<wasm_localcse_t> wasm_localcsv_t;


struct FuncDecl;

struct SubsecBytes {
  byte id;
  bytearr bytes;
};
struct DebugNameAssoc {
  FuncDecl* func;
  std::string name;
};
struct DebugNameDecl {
  // Everything except function subsection
  std::list <SubsecBytes> subsections;
  // Function subsection: Id 1
  std::list <DebugNameAssoc> func_assoc;
};

/* Section Field Declarations */
struct CustomDecl {
  std::string name;
  bytearr bytes;
  /* Only populated for 'name' section */
  DebugNameDecl debug;
};


struct SigDecl {
  typelist params;
  typelist results;

  bool operator==(const SigDecl &sig) {
    bool parameq = std::equal(this->params.begin(), this->params.end(),
            sig.params.begin(), sig.params.end());
    bool resulteq = std::equal(this->results.begin(), this->results.end(),
            sig.results.begin(), sig.results.end());
    return parameq && resulteq;
  }
  bool operator!=(const SigDecl &sig) {
    bool parameq = std::equal(this->params.begin(), this->params.end(),
            sig.params.begin(), sig.params.end());
    bool resulteq = std::equal(this->results.begin(), this->results.end(),
            sig.results.begin(), sig.results.end());
    return !(parameq && resulteq);
  }
};


struct FuncDecl {
  /* From func/import section */
  SigDecl* sig;
  /* From code section */
  wasm_localcsv_t pure_locals;
  uint32_t num_pure_locals;
  bytearr code_bytes;
  /* Code: Raw static instructions */ 
  InstList instructions;

  uint32_t add_local(wasm_type_t new_type) {
    wasm_localcse_t &last = this->pure_locals.back();
    if (last.type == new_type) {
      last.count++;
    } else {
      this->pure_locals.push_back( { .count = 1, .type = new_type } );
    }
    uint32_t idx = this->sig->params.size() + num_pure_locals;
    num_pure_locals++;
    return idx;
  }
};


struct MemoryDecl {
  wasm_limits_t limits;
};


struct TableDecl {
  wasm_type_t reftype;
  wasm_limits_t limits;
};


struct GlobalInfo {
  wasm_type_t type;
  unsigned is_mutable : 1;
};
struct GlobalDecl {
  wasm_type_t type;
  unsigned is_mutable : 1;
  bytearr init_expr_bytes;
};


struct DataDecl {
  uint32_t flag;
  Opcode_t opcode_offset;
  uint32_t mem_offset;
  MemoryDecl *mem;
  bytearr bytes;
};


struct ElemDecl {
  uint32_t flag;
  Opcode_t opcode_offset;
  uint32_t table_offset;
  std::list <FuncDecl*> func_indices;
};


union Descriptor {
  FuncDecl* func;
  GlobalDecl* global;
  TableDecl* table;
  MemoryDecl* mem;
};

struct ImportInfo {
  std::string mod_name;
  std::string member_name;
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


struct ImportSet {
  std::list <ImportDecl> list;
  uint32_t num_funcs;
  uint32_t num_tables;
  uint32_t num_mems;
  uint32_t num_globals;

  inline uint32_t get_num_imports () {
    return num_funcs + num_tables + num_mems + num_globals;
  }
};


/* Section */
class WasmModule {

  private:
    uint32_t magic;
    uint32_t version;

    std::list <CustomDecl>  customs;
    std::list <SigDecl>     sigs;
    ImportSet               imports;
    /* Func space */
    std::deque <FuncDecl>    funcs;
    /* Table space */
    std::list <TableDecl>   tables;
    /* Mem space */
    std::list <MemoryDecl>  mems;
    /* Global space */
    std::deque <GlobalDecl>  globals;
    std::list <ExportDecl>  exports;
    std::list <ElemDecl>    elems;
    std::list <DataDecl>    datas;

    /* Start section */
    FuncDecl* start_fn;

    /* Datacount section */ 
    int has_datacount;
    /* For decode validation only, don't use otherwise */
    int num_datas_datacount;

    /* Custom name section debug reference */
    std::list <DebugNameAssoc> *fn_names_debug;

    /* Decode functions */
    #define DECODE_DECL(sec,...)  \
      void decode_##sec##_section (buffer_t &buf, uint32_t len __VA_OPT__(,) __VA_ARGS__);
    DECODE_DECL(type);
    DECODE_DECL(import);
    DECODE_DECL(function);
    DECODE_DECL(table);
    DECODE_DECL(memory);
    DECODE_DECL(global);
    DECODE_DECL(export);
    DECODE_DECL(start);
    DECODE_DECL(element);
    DECODE_DECL(code, bool gen_cfg);
    DECODE_DECL(data);
    DECODE_DECL(datacount);
    DECODE_DECL(custom);

    /* Encode functions */
    #define ENCODE_DECL(sec, ...)  \
      bytedeque encode_##sec##_section (__VA_ARGS__);
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
    ENCODE_DECL(custom, CustomDecl &custom);

    /* Code decoding for instructions */
    InstList decode_expr_to_insts (buffer_t &buf, bool gen_cfg);
    /* Code encoding for instructions */
    void encode_expr_to_insts (bytedeque &bdeq, InstList &instlist, bytearr &bytes);
    /* Code + local encoding for instructions */
    bytedeque encode_code (FuncDecl &func);

    /* Descriptor patching for copy/assign */
    template<typename T>
    void DescriptorPatch (std::list<T> &list, const WasmModule &mod, std::unordered_map<void*, void*> &reassign_cache);
    /* Function patching for copy/assign */
    void FunctionPatch (const WasmModule &mod, std::unordered_map<void*, void*> &reassign_cache);
    /* Custom section patching for copy/assign */
    void CustomPatch (const WasmModule &mod, std::unordered_map<void*, void*> &reassign_cache);

    /* Perform a full deep copy */
    WasmModule& deepcopy(const WasmModule &mod, const char* log_str);

  public:
    WasmModule () = default;
    WasmModule (const WasmModule &mod);
    ~WasmModule() = default;

    WasmModule& operator=(const WasmModule &mod);

    /* Field Accessors */
    inline SigDecl* getSig(uint32_t idx)        { return GET_LIST_ELEM(this->sigs, idx); }
    inline FuncDecl* getFunc(uint32_t idx)      { return GET_DEQUE_ELEM(this->funcs, idx); }
    inline GlobalDecl* getGlobal(uint32_t idx)  { return GET_DEQUE_ELEM(this->globals, idx); }
    inline TableDecl* getTable(uint32_t idx)    { return GET_LIST_ELEM(this->tables, idx); }
    inline MemoryDecl* getMemory(uint32_t idx)  { return GET_LIST_ELEM(this->mems, idx); }
    inline DataDecl* getData(uint32_t idx)      { return GET_LIST_ELEM(this->datas, idx); }
    inline ImportDecl* getImport(uint32_t idx)  { return GET_LIST_ELEM(this->imports.list, idx); }

    /* Index Accessors */
    inline uint32_t getSigIdx(SigDecl *sig)           const { return GET_LIST_IDX(this->sigs, sig); }
    inline uint32_t getFuncIdx(FuncDecl *func)        const { return GET_DEQUE_IDX(this->funcs, func); }
    inline uint32_t getGlobalIdx(GlobalDecl *global)  const { return GET_DEQUE_IDX(this->globals, global); }
    inline uint32_t getTableIdx(TableDecl *table)     const { return GET_LIST_IDX(this->tables, table); }
    inline uint32_t getMemoryIdx(MemoryDecl *mem)     const { return GET_LIST_IDX(this->mems, mem); }
    inline uint32_t getDataIdx(DataDecl *data)        const { return GET_LIST_IDX(this->datas, data); }
    inline uint32_t getImportIdx(ImportDecl *import)  const { return GET_LIST_IDX(this->imports.list, import); }

    /* Import accessors */
    inline bool isImport(FuncDecl *func)      { return getFuncIdx(func)     < this->imports.num_funcs; }
    inline bool isImport(GlobalDecl *global)  { return getGlobalIdx(global) < this->imports.num_globals; }
    inline bool isImport(TableDecl *table)    { return getTableIdx(table)   < this->imports.num_tables; }
    inline bool isImport(MemoryDecl *mem)     { return getMemoryIdx(mem)    < this->imports.num_mems; }

    /* Section Accessors */
    inline std::deque <FuncDecl> &Funcs() { return this->funcs; }
    inline std::deque <GlobalDecl> &Globals() { return this->globals; }

    inline FuncDecl* get_start_fn() { return this->start_fn; }
    inline uint32_t get_num_customs() { return this->customs.size(); }

    /* Debug Accessor */
    inline std::list<DebugNameAssoc>* getFnDebugNames() { return this->fn_names_debug; }

    /* Scope view of function code */
    ScopeList gen_scopes_from_instructions(FuncDecl *func);

    /* Decode wasm file from buffer */
    void decode_buffer (buffer_t &buf, bool gen_cfg);
    /* Encode module into wasm format */
    bytedeque encode_module (char* outfile);


    /* Instrumentation methods */
    
    /* Addition */
    /* Module-internal values with optional export */
    SigDecl*    add_sig     (SigDecl &sig, bool force_dup = false);
    GlobalDecl* add_global  (GlobalDecl &global, const char* export_name = NULL);
    TableDecl*  add_table   (TableDecl &table, const char* export_name = NULL);
    MemoryDecl* add_memory  (MemoryDecl &mem, const char* export_name = NULL);
    FuncDecl*   add_func    (FuncDecl &func, const char* export_name = NULL, const char* debug_name = NULL);

    /* Imported values */
    ImportDecl* add_import (ImportInfo &info, GlobalInfo &global);
    ImportDecl* add_import (ImportInfo &info, TableDecl &table);
    ImportDecl* add_import (ImportInfo &info, MemoryDecl &mem);
    ImportDecl* add_import (ImportInfo &info, SigDecl &sig);

    /* Exported values */
    ExportDecl* add_export (std::string export_name, GlobalDecl &global);
    ExportDecl* add_export (std::string export_name, TableDecl &table);
    ExportDecl* add_export (std::string export_name, MemoryDecl &mem);
    ExportDecl* add_export (std::string export_name, FuncDecl &func);

    /* Find */
    ExportDecl* find_export (std::string export_name);

    /* Replace uses: UNIMPLEMENTED */
    void replace_all_uses (GlobalDecl* old_inst, GlobalDecl* new_inst);
    void replace_all_uses (TableDecl* old_inst, TableDecl* new_inst);
    void replace_all_uses (MemoryDecl* old_inst, MemoryDecl* new_inst);
    void replace_all_uses (FuncDecl* old_inst, FuncDecl* new_inst);
};


