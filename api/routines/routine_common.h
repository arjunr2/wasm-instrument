#ifndef ROUTINE_COMMON_H
#define ROUTINE_COMMON_H

#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <map>
#include <unordered_set>

#include "common.h"
#include "ir.h"
#include "views.h"
#include "instructions.h"


/* A Builder Class that enables easy generation of instrumented logic */
class InstBuilder {
    private:
        InstList buf;
    public:
        InstBuilder(std::initializer_list<InstBasePtr> xargs) {
          buf.insert(buf.end(), xargs.begin(), xargs.end());
        }

        inline InstList& get_buf() { return buf; }

        /* Pushing single instruction */
        template<typename T>
        void push_inst (T inst) {
          InstBasePtr x = std::make_shared<T>(inst);
          buf.push_back(x);
        }

        inline void push_inst (InstBasePtr instptr) {
          buf.push_back(instptr);
        }

        /* Pushing list of instructions */
        void push(std::initializer_list<InstBasePtr> xargs) {
          for (auto x : xargs) {
              this->push_inst(x);
          }
        }

        /* Splices the list (default to the end) */
        inline void splice(InstBuilder &other) {
          buf.splice(this->buf.end(), other.get_buf());
        }

        inline void splice(InstBuilder &other, InstList::iterator pos) {
          buf.splice(pos, other.get_buf());
        }

        inline void splice_into(InstList &other, InstList::iterator pos) {
          other.splice(pos, buf);
        }

        /* Size */
        inline bool empty() {
          return buf.empty();
        }
};

// Wasm locals representation for instrumentation
struct LocalVal {
	wasm_type_t type;
	uint32_t idx;

    public:
        LocalVal() : type((wasm_type_t)0), idx(-1) {}
        LocalVal(wasm_type_t t, uint32_t i) : type(t), idx(i) {}
};

// Allocator interface for Wasm locals
class LocalAllocator {
    private:
        std::map<wasm_type_t, std::vector<uint32_t>> idx_map;
        std::map<wasm_type_t, uint32_t> allocator;

    public:
        // Initialize local index map from the locals count
        LocalAllocator(FuncDecl *func, std::map<wasm_type_t, uint32_t> locals_count) {
            idx_map.clear();
            if (func != NULL) {
              for (auto &x: locals_count) {
                  idx_map[x.first] = std::vector<uint32_t>(x.second);
                  for (int i = 0; i < x.second; i++) {
                      idx_map[x.first][i] = func->add_local(x.first);
                  }
              }
            }
        }

        // Reset all allocations
        inline void reset_allocator() {
            for (auto &x: allocator) { x.second = 0; }
        }

        // Allocate a local of specific type
        LocalVal allocate(wasm_type_t type) {
            auto type_idx_vector = idx_map.at(type);
            if (allocator[type] >= type_idx_vector.size()) {
                ERR("Local allocation exceeded for type %d\n", type);
                return LocalVal();
            }
            return LocalVal(type, type_idx_vector[allocator[type]++]);
            //return (LocalVal) { .type = type, .idx = type_entry[allocator[type]++] };
        }

        // Get the total number of locals allocatable across all types
        inline uint32_t get_local_pool_count() {
            uint32_t max = 0;
            for (auto &x: idx_map) { max += x.second.size(); }
            return max;
        }
        
        // Get immutable reference to index map
        const std::map<wasm_type_t, std::vector<uint32_t>>& get_idx_map() {
            return idx_map;
        }
        // Get immutable reference to type vector in index map
        const std::vector<uint32_t>& get_type_vector(wasm_type_t type) {
            return idx_map[type];
        }
};

// Instructions to typecast any type to I64
InstBuilder builder_push_i64_extend(InstBuilder &builder, wasm_type_t type);

/* Method to look for either main or _start */
ExportDecl* get_main_export(WasmModule &module);

/* Add pages to memory statically. Return the old number of pages */
uint32_t add_pages(MemoryDecl *mem, uint32_t num_pages);

// Core instrumentation for call_indirect interposition
// Update elems with funcref (works only if module uses active elems):
// `call_indirect`s will go through our instrumented elem targets, allowing us
// interpose on its capability dynamically.
// Setting record_phase = true allows passing an additional I32 param
std::set<FuncDecl*> instrument_funcref_elems (WasmModule &module, 
      std::vector<wasm_type_t> marshall_vals = {});

// Method to insert instructions at the end of execution of entry function or relevant import funcs
void insert_on_exit(WasmModule &module, FuncDecl *entry_func, InstBuilder &builder, 
  std::vector<ImportInfo> import_exits = {});
  
#endif
