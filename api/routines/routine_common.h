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
};


/* Method to look for either main or _start */
ExportDecl* get_main_export(WasmModule &module);


#endif