#pragma once

#include <list>
#include "ir_defs.h"
#include "ir.h"

/* An instruction builder for instrumentation */
class InstBuilder {
    private:
        InstList buf;
    public:
        InstBuilder(std::initializer_list<InstBasePtr> xargs) {
          buf.insert(buf.end(), xargs.begin(), xargs.end());
        }

        /* --- Primitives --- */
        inline InstList& get_buf() { return buf; }

        // Push: Single instruction
        inline InstBuilder& push_inst (InstBasePtr instptr) { 
          buf.push_back(instptr); 
          return *this;
        }
        template<typename T> 
        inline InstBuilder& push_inst (T inst) {
          InstBasePtr x = std::make_shared<T>(inst);
          buf.push_back(x);
          return *this;
        }

        // Push: List of instructions
        inline InstBuilder& push(std::initializer_list<InstBasePtr> xargs) {
          for (auto x : xargs) { this->push_inst(x); }
          return *this;
        }

        // Splices the list (default to the end)
        inline void splice(InstBuilder &other) { 
          buf.splice(this->buf.end(), other.get_buf());
        }
        inline void splice(InstBuilder &other, InstList::iterator pos) { 
          buf.splice(pos, other.get_buf());
        }
        inline void splice_into(InstList &other, InstList::iterator pos) { 
          other.splice(pos, buf); 
        }

        // Empty check
        inline bool empty() { return buf.empty(); }
        /* --- */

        /* --- Common Patterns --- */
        // Push typecast top of stack to I64
        InstBuilder& i64_convert(wasm_type_t type);
        // Push <type>.ne instruction
        InstBuilder& ne(wasm_type_t type);
        // Push i64.store<size> instruction; size is in bytes
        InstBuilder& i64_store(uint32_t size, MemoryDecl *mem);
        // Push <type>.const instruction from i64 for scalar types
        InstBuilder& scalar_const(wasm_type_t type, int64_t val64);
        /* --- */
};