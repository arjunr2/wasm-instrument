#ifndef R3_COMMON_H
#define R3_COMMON_H

#include "routine_common.h"

/* Types of Call instructions */
typedef enum {
  SC_UNKNOWN = 0,
  /* Specialized Calls */
  SC_MMAP,
  SC_WRITEV,
  /* Lockless Calls */
  SC_THREAD_SPAWN,
  SC_FUTEX,
  SC_EXIT,
  SC_PROC_EXIT,
  /* All other generic imports */
  SC_GENERIC
} CallID;

typedef enum {
  RET_PH = 0,
  RET_VOID,
  RET_I32,
  RET_I64,
  RET_F32,
  RET_F64
} WasmRet;

static const char* ignored_export_funcs[] = {
  "wasm_memory_grow",
  "wasm_memory_size"
};

static bool set_func_export_map(WasmModule &module, std::string name, std::map<FuncDecl*, std::string>& export_map) {
  ExportDecl *exp = module.find_export(name);
  if (exp && exp->kind == KIND_FUNC) {
    export_map[exp->desc.func] = name;
    return true;
  }
  return false;
}

/* Record currently inserts 2 import functions, making the function indices 
  we get at replay time offset by 2.
  This method performs that transformation */
static uint32_t transform_to_record_func_idx(uint32_t replay_idx) {
  return replay_idx + 2;
}

#endif