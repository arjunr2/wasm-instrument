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
  int depth;
  int loop_depth;

  /* N-level subscope depth */
  std::list<ScopeBlock*> subscopes;
  /* One level subscope-depth: this is a subset of subscopes above */
  std::list<ScopeBlock*> outer_subscopes;
  
  ScopeBlock () { };
  ScopeBlock (InstItr start, int depth) { 
    this->start = start;
    this->depth = depth;
  }

  inline void set_end(InstItr end) { this->end = end; }
  inline void set_loop_depth(int loop_depth) { this->loop_depth = loop_depth; }

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

inline static bool isScopeInst(InstBasePtr inst) {
  switch (inst->getOpcode()) {
    case WASM_OP_LOOP:
    case WASM_OP_BLOCK:
    case WASM_OP_IF:
    case WASM_OP_ELSE:
    case WASM_OP_END:
      return true;
    default:
      return false;
  }
  return false;
}