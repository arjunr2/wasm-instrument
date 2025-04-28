#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <sys/syscall.h>
#include <unordered_map>

#include "common.h"
#include "ir_defs.h"
#include "wasmdefs.h"

/* Utility Functions */
const char *wasm_type_string(wasm_type_t type);
const char *wasm_section_name(byte sec_code);
const char *wasm_kind_string(wasm_kind_t kind);

class WasmModule;
class FuncDecl;
class InstBuilder;

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
    FuncDecl *func;
    std::string name;
};
struct DebugNameDecl {
    // Everything except function subsection
    std::list<SubsecBytes> subsections;
    // Function subsection: Id 1
    std::list<DebugNameAssoc> func_assoc;
};

/* Section Field Declarations */
struct CustomDecl {
    std::string name;
    bytearr bytes;
    /* Only populated for 'name' section */
    DebugNameDecl debug;
};

struct SigDecl {
    TypeList params;
    TypeList results;

    bool operator==(const SigDecl &sig) const {
        bool parameq = std::equal(this->params.begin(), this->params.end(),
                                  sig.params.begin(), sig.params.end());
        bool resulteq = std::equal(this->results.begin(), this->results.end(),
                                   sig.results.begin(), sig.results.end());
        return parameq && resulteq;
    }
    bool operator!=(const SigDecl &sig) const {
        bool parameq = std::equal(this->params.begin(), this->params.end(),
                                  sig.params.begin(), sig.params.end());
        bool resulteq = std::equal(this->results.begin(), this->results.end(),
                                   sig.results.begin(), sig.results.end());
        return !(parameq && resulteq);
    }
};
// Hash function class for SigDecl
class SigDeclHash {
  public:
    size_t operator()(const SigDecl &s) const {
        size_t hs = 0;
        for (auto &x : s.params) {
            hs += (size_t)x;
        }
        for (auto &x : s.params) {
            hs += (size_t)x;
        }
        return hs;
    }
};

struct FuncDecl {
    /* From func/import section */
    SigDecl *sig;
    /* From code section */
    wasm_localcsv_t pure_locals;
    uint32_t num_pure_locals;
    bytearr code_bytes;
    /* Code: Raw static instructions */
    InstList instructions;

    uint32_t add_local(wasm_type_t new_type);
};

struct MemoryDecl {
    wasm_limits_t limits;

    uint32_t add_pages(uint32_t num_pages);
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
    std::list<FuncDecl *> funcs;
};

union Descriptor {
    FuncDecl *func;
    GlobalDecl *global;
    TableDecl *table;
    MemoryDecl *mem;
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
    std::list<ImportDecl> list;
    uint32_t num_funcs;
    uint32_t num_tables;
    uint32_t num_mems;
    uint32_t num_globals;

    inline uint32_t get_num_imports() {
        return num_funcs + num_tables + num_mems + num_globals;
    }
};

/* Section */
class WasmModule {

  private:
    uint32_t magic;
    uint32_t version;

    std::list<CustomDecl> customs;
    std::list<SigDecl> sigs;
    ImportSet imports;
    /* Func space */
    std::list<FuncDecl> funcs;
    /* Table space */
    std::list<TableDecl> tables;
    /* Mem space */
    std::list<MemoryDecl> mems;
    /* Global space */
    std::deque<GlobalDecl> globals;
    std::list<ExportDecl> exports;
    std::list<ElemDecl> elems;
    std::list<DataDecl> datas;

    /* Start section */
    FuncDecl *start_fn;

    /* Datacount section */
    int has_datacount;
    /* For decode validation only, don't use otherwise */
    int num_datas_datacount;

    /* Custom name section debug reference */
    std::list<DebugNameAssoc> *fn_names_debug;

    /* --- Parsing and Encoding --- */
    // Decode functions
    void decode_type_section(buffer_t &buf, uint32_t len);
    void decode_import_section(buffer_t &buf, uint32_t len);
    void decode_function_section(buffer_t &buf, uint32_t len);
    void decode_table_section(buffer_t &buf, uint32_t len);
    void decode_memory_section(buffer_t &buf, uint32_t len);
    void decode_global_section(buffer_t &buf, uint32_t len);
    void decode_export_section(buffer_t &buf, uint32_t len);
    void decode_start_section(buffer_t &buf, uint32_t len);
    void decode_element_section(buffer_t &buf, uint32_t len);
    void decode_code_section(buffer_t &buf, uint32_t len, bool gen_cfg);
    void decode_data_section(buffer_t &buf, uint32_t len);
    void decode_datacount_section(buffer_t &buf, uint32_t len);
    void decode_custom_section(buffer_t &buf, uint32_t len);

    // Encode functions
    bytedeque encode_type_section();
    bytedeque encode_import_section();
    bytedeque encode_function_section();
    bytedeque encode_table_section();
    bytedeque encode_memory_section();
    bytedeque encode_global_section();
    bytedeque encode_export_section();
    bytedeque encode_start_section();
    bytedeque encode_element_section();
    bytedeque encode_code_section();
    bytedeque encode_data_section();
    bytedeque encode_datacount_section();
    bytedeque encode_custom_section(CustomDecl &custom);

    // Code decoding for instructions
    InstList decode_expr_to_insts(buffer_t &buf, bool gen_cfg);
    // Code encoding for instructions
    void encode_expr_to_insts(bytedeque &bdeq, InstList &instlist,
                              bytearr &bytes);
    // Code + local encoding for instructions
    bytedeque encode_code(FuncDecl &func);
    // Instrumentation helper methods
    bool remove_func_core(uint32_t idx, FuncDecl *func);
    /* --- */

    /* --- Deep-Copy Support --- */
    // Descriptor patching for copy/assign
    template <typename T>
    void DescriptorPatch(std::list<T> &list, const WasmModule &mod,
                         std::unordered_map<void *, void *> &reassign_cache);
    // Function patching for copy/assign
    void FunctionPatch(const WasmModule &mod,
                       std::unordered_map<void *, void *> &reassign_cache);
    // Custom section patching for copy/assign
    void CustomPatch(const WasmModule &mod,
                     std::unordered_map<void *, void *> &reassign_cache);
    // Perform a full deep copy
    WasmModule &deepcopy(const WasmModule &mod, const char *log_str);
    /* --- */

  public:
    WasmModule() = default;
    WasmModule(const WasmModule &mod);
    ~WasmModule() = default;

    WasmModule &operator=(const WasmModule &mod);

    // Decode wasm file from buffer
    void decode_buffer(buffer_t &buf, bool gen_cfg);
    // Encode module into wasm format
    bytedeque encode_module(char *outfile);

    // Field Accessors
    inline SigDecl *getSig(uint32_t idx) {
        return GET_LIST_ELEM(this->sigs, idx);
    }
    inline FuncDecl *getFunc(uint32_t idx) {
        return GET_LIST_ELEM(this->funcs, idx);
    }
    inline GlobalDecl *getGlobal(uint32_t idx) {
        return GET_DEQUE_ELEM(this->globals, idx);
    }
    inline TableDecl *getTable(uint32_t idx) {
        return GET_LIST_ELEM(this->tables, idx);
    }
    inline MemoryDecl *getMemory(uint32_t idx) {
        return GET_LIST_ELEM(this->mems, idx);
    }
    inline DataDecl *getData(uint32_t idx) {
        return GET_LIST_ELEM(this->datas, idx);
    }
    inline ImportDecl *getImport(uint32_t idx) {
        return GET_LIST_ELEM(this->imports.list, idx);
    }

    // Index Accessors
    inline uint32_t getSigIdx(SigDecl *sig) const {
        return GET_LIST_IDX(this->sigs, sig);
    }
    inline uint32_t getFuncIdx(FuncDecl *func) const {
        return GET_LIST_IDX(this->funcs, func);
    }
    inline uint32_t getGlobalIdx(GlobalDecl *global) const {
        return GET_DEQUE_IDX(this->globals, global);
    }
    inline uint32_t getTableIdx(TableDecl *table) const {
        return GET_LIST_IDX(this->tables, table);
    }
    inline uint32_t getMemoryIdx(MemoryDecl *mem) const {
        return GET_LIST_IDX(this->mems, mem);
    }
    inline uint32_t getDataIdx(DataDecl *data) const {
        return GET_LIST_IDX(this->datas, data);
    }
    inline uint32_t getImportIdx(ImportDecl *import) const {
        return GET_LIST_IDX(this->imports.list, import);
    }

    // Import accessors
    inline bool isImport(FuncDecl *func) {
        return getFuncIdx(func) < this->imports.num_funcs;
    }
    inline bool isImport(GlobalDecl *global) {
        return getGlobalIdx(global) < this->imports.num_globals;
    }
    inline bool isImport(TableDecl *table) {
        return getTableIdx(table) < this->imports.num_tables;
    }
    inline bool isImport(MemoryDecl *mem) {
        return getMemoryIdx(mem) < this->imports.num_mems;
    }

    inline uint32_t getNumImportFuncs() { return this->imports.num_funcs; }

    // Section Accessors
    inline std::list<FuncDecl> &Funcs() { return this->funcs; }
    inline std::deque<GlobalDecl> &Globals() { return this->globals; }
    inline std::list<ExportDecl> &Exports() { return this->exports; }
    inline std::list<ElemDecl> &Elems() { return this->elems; }

    inline FuncDecl *get_start_fn() { return this->start_fn; }
    inline uint32_t get_num_customs() { return this->customs.size(); }

    // Debug Accessor
    inline std::list<DebugNameAssoc> *getFnDebugNames() {
        return this->fn_names_debug;
    }

    // Scope view of function code
    ScopeList gen_scopes_from_instructions(FuncDecl *func);
    // Same as above method, but only return the main scope block
    ScopeBlock gen_func_static_scope_hierarchy(FuncDecl *func);

    /* --- Instrumentation Primitives --- */
    SigDecl *add_sig(SigDecl &sig, bool force_dup = false);
    GlobalDecl *add_global(GlobalDecl &global, const char *export_name = NULL);
    TableDecl *add_table(TableDecl &table, const char *export_name = NULL);
    MemoryDecl *add_memory(MemoryDecl &mem, const char *export_name = NULL);
    FuncDecl *add_func(FuncDecl &func, const char *export_name = NULL,
                       const char *debug_name = NULL);

    ImportDecl *add_import(ImportInfo &info, GlobalInfo &global);
    ImportDecl *add_import(ImportInfo &info, TableDecl &table);
    ImportDecl *add_import(ImportInfo &info, MemoryDecl &mem);
    ImportDecl *add_import(ImportInfo &info, SigDecl &sig);

    ExportDecl *add_export(std::string export_name, GlobalDecl &global);
    ExportDecl *add_export(std::string export_name, TableDecl &table);
    ExportDecl *add_export(std::string export_name, MemoryDecl &mem);
    ExportDecl *add_export(std::string export_name, FuncDecl &func);

    ExportDecl *find_export(std::string export_name);
    FuncDecl *find_func_from_debug_name(std::string debug_name);

    GlobalDecl *find_import_global(std::string mod_name,
                                   std::string member_name);
    TableDecl *find_import_table(std::string mod_name, std::string member_name);
    MemoryDecl *find_import_memory(std::string mod_name,
                                   std::string member_name);
    FuncDecl *find_import_func(std::string mod_name, std::string member_name);

    ImportDecl *get_import_name_from_func(FuncDecl *func);
    std::pair<bool, std::string &> get_debug_name_from_func(FuncDecl *func);

    bool remove_func(uint32_t idx);
    bool remove_func(FuncDecl *func);
    /* --- */

    /* --- Common Instrumentation Patterns --- */
    FuncDecl *get_main_fn();

    std::set<FuncDecl *>
    funcref_wrap(std::vector<wasm_type_t> marshall_vals = {});

    void exit_instrument(InstBuilder &builder,
                         std::vector<ImportInfo> import_func_exits = {},
                         FuncDecl *entry_func = NULL);

    void
    beforeFuncCall(std::map<FuncDecl *, DirectCallSiteCallback> &builder_map);
    void beforeImmTypeInstruction(opcode_imm_type opcode,
                                  InstructionSiteCallback callback);
};
