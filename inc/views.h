#pragma once
#include "common.h"
#include "wasmdefs.h"
#include "ir.h"
#include "inst_internal.h"

typedef enum {
  LOOP,
  BLOCK,
  IF,
  IFWELSE,
  ELSE,
  INVALID
} scope_type_t;

/* Scope view of a function code */
struct ScopeBlock {
  InstItr start;
  InstItr end;

  /* N-level subscope depth */
  std::list<ScopeBlock*> subscopes;
  /* One level subscope-depth: this is a subset of subscopes above */
  std::list<ScopeBlock*> outer_subscopes;
  
  ScopeBlock () { };
  ScopeBlock (InstItr start) { this->start = start; }

  inline void set_end(InstItr end) { this->end = end; }

  scope_type_t get_scope_type() {
    InstBasePtr startinst = *start;
    switch (startinst->getOpcode()) {
      case WASM_OP_LOOP: return LOOP;
      case WASM_OP_BLOCK: return BLOCK;
      case WASM_OP_IF: {
            if ((*end)->getOpcode() != WASM_OP_END) { return IFWELSE; }
            else return IF;
      }
      case WASM_OP_ELSE: return ELSE;
    }
    return INVALID;
  }
};
