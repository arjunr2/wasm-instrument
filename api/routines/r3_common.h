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

/* Types of Return Values for recorded instructions */
typedef enum {
  RET_PH = 0,
  RET_VOID,
  RET_I32,
  RET_I64,
  RET_F32,
  RET_F64
} WasmRet;


/** Special function data for record-replay */
/* List of export functions to ignore during recording */
static const char* ignored_export_funcnames[] = {
  "wasm_memory_grow",
  "wasm_memory_size"
};
/* List of recording functions that require additional special operations 
  at replay */
static const char *specialized_import_names[2] = {"SYS_mmap", "SYS_writev"};
    
/* List of recording functions that cannot use lock-based instrumentation. 
  Each are handled in a specialized manner 
  NOTE: proc_exit and exit_group are identical in behavior
  */
static const char *lockless_import_names[5] = {
  "__wasm_thread_spawn", "SYS_futex", "SYS_exit", 
    "__proc_exit", "SYS_exit_group"};
/** */


static WasmRet to_wasmret(wasm_type_t type) {
  switch (type) {
    case WASM_TYPE_I32: return RET_I32;
    case WASM_TYPE_I64: return RET_I64;
    case WASM_TYPE_F32: return RET_F32;
    case WASM_TYPE_F64: return RET_F64;
    default: ERR("Type %d does not map to any retval type\n", type);
      return RET_PH;
  }

}

/* Data to initialize import functions */
typedef struct {
  ImportInfo iminfo;
  SigDecl sig;
  uint32_t key;
} ImportFuncData;


/** Record Instrumentation Functions */
#define NUM_RECORD_IMPORTS 2
static ImportFuncData record_imports[NUM_RECORD_IMPORTS] = {
  { // Tracing function for memory operations
    .iminfo = {
      .mod_name = "instrument",
      .member_name = "memop_tracedump"
    },
    .sig = {
      .params = {
        // Differ?
        WASM_TYPE_I32, 
        // Access Index
        WASM_TYPE_I32, 
        // Opcode
        WASM_TYPE_I32, 
        // Address
        WASM_TYPE_I32, 
        // Size/Width of Access
        WASM_TYPE_I32,
        // Load value from Main Memory (only used if Differ == 1) 
        WASM_TYPE_I64, 
        // Expected value from Main Memory (only used if Differ == 1) 
        WASM_TYPE_I64
      },
      .results = {} 
    }
  },
  { // Tracing function for import function calls
    .iminfo = {
      .mod_name = "instrument",
      .member_name = "call_tracedump"
    },
    .sig = {
      .params = {
        // Access Index
        WASM_TYPE_I32, 
        // Opcode
        WASM_TYPE_I32, 
        // Function Index
        WASM_TYPE_I32, 
        // Call ID
        WASM_TYPE_I32,
        // Return value (up to 1)
        WASM_TYPE_I64,
        // Arguments (up to 3)
        WASM_TYPE_I64, WASM_TYPE_I64, WASM_TYPE_I64,
      },
      .results = {} 
    }
  }
};
/**  */

/** Replay Instrumentation Functions */
#define NUM_REPLAY_IMPORTS 2
static ImportFuncData replay_imports[NUM_REPLAY_IMPORTS] = {
  { // writev call replay
    .iminfo = {
      .mod_name = "r3-replay",
      .member_name = "SC_writev"
    },
    .sig = {
      .params = {
        // File descriptor
        WASM_TYPE_I32, 
        // Iovec pointer
        WASM_TYPE_I32, 
        // Iovec count
        WASM_TYPE_I32, 
      },
      .results = {
        // Return value
        WASM_TYPE_I64
      } 
    },
    .key = SC_WRITEV
  },
  { // proc_exit call replay
    .iminfo = {
      .mod_name = "r3-replay",
      .member_name = "SC_proc_exit"
    },
    .sig = {
      .params = {
        // Error Code
        WASM_TYPE_I32, 
      },
      .results = { } 
    },
    .key = SC_PROC_EXIT
  },
};
/** */

static FuncDecl* add_r3_import_function(WasmModule &module, ImportFuncData &imdata) {
  ImportDecl* import_decl = module.add_import(imdata.iminfo, imdata.sig);
  return import_decl->desc.func;
}

/* Populate export map with the name:FuncDecl* pair */
static bool set_func_export_map(WasmModule &module, std::string name, std::map<FuncDecl*, std::string>& export_map) {
  ExportDecl *exp = module.find_export(name);
  if (exp && exp->kind == KIND_FUNC) {
    export_map[exp->desc.func] = name;
    return true;
  }
  return false;
}

/* Record currently inserts the import functions (call_tracedump & memop_tracedump),
  making the function indices we get at replay time offset by the inserted amount.
  This method performs a transformation from the index for a specific function
  at replay time to that at record time */
static uint32_t transform_to_record_func_idx(uint32_t replay_idx) {
  return replay_idx + NUM_RECORD_IMPORTS - NUM_REPLAY_IMPORTS;
}

#endif