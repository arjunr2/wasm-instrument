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
#include "builder.h"


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

#endif
