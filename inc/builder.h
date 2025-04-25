#pragma once

#include <list>
#include "ir_defs.h"

/* A Builder Class that enables easy generation of instrumented logic */
class InstBuilder {
    private:
        InstList buf;
    public:
        InstBuilder(std::initializer_list<InstBasePtr> xargs) {
          buf.insert(buf.end(), xargs.begin(), xargs.end());
        }

        inline InstList& get_buf() { return buf; }

        // Pushing single instruction
        template<typename T>
        void push_inst (T inst) {
          InstBasePtr x = std::make_shared<T>(inst);
          buf.push_back(x);
        }

        inline void push_inst (InstBasePtr instptr) {
          buf.push_back(instptr);
        }

        // Pushing list of instructions
        void push(std::initializer_list<InstBasePtr> xargs) {
          for (auto x : xargs) {
              this->push_inst(x);
          }
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

        /* Size */
        inline bool empty() {
          return buf.empty();
        }
};