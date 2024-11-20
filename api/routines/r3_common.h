#ifndef R3_COMMON_H
#define R3_COMMON_H

#include "routine_common.h"
#include "r3_memops_table.h"

namespace RecordInterface {
  /* Types of Call instructions */
  typedef enum {
    SC_UNKNOWN = 0,
    /* Specialized Calls */
    SC_MMAP = 1,
    SC_WRITEV = 2,
    SC_THREAD_SPAWN = 3,
    /* Lockless Calls */
    SC_FUTEX = 4,
    SC_THREAD_EXIT = 5,
    SC_PROC_EXIT = 6,
    /* All other generic imports */
    SC_GENERIC = 0xFFFFFFFF
  } CallID;

  typedef enum {
    FutexWait = 0,
    FutexWake = 1,
    FutexUnknown = -1
  } FutexOp;
}

namespace ReplayInterface {
  /* Replay-specific call instructions 
  Enums must not collide with RecordInterface */
  typedef enum {
    SC_LOG_CALL = 0x70000000,
    SC_GETTID = 0x70000001,
  } CallID;
}

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
/* List of functions to ignore by debug name */
static const char* ignored_debug_funcnames[] = {
  "__wasm_init_memory" 
};
/* List of recording functions that require additional special operations 
  at replay */
static const char *specialized_import_names[3] = {"SYS_mmap", "SYS_writev",
  "__wasm_thread_spawn" };
    
/* List of recording functions that cannot use lock-based instrumentation. 
  Each are handled in a specialized manner 
  NOTE: proc_exit and exit_group are identical in behavior
  */
static const char *lockless_import_names[4] = {
  // Futex doesn't need lock since it doesn't need to be ordered w.r.t. memory accesses
  "SYS_futex", 
  "SYS_exit", 
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
  bool debug;
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
        WASM_TYPE_I64,
        // Flag as implicit synchronization operator
        WASM_TYPE_I32
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
#define NUM_REPLAY_IMPORTS 7
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
    .key = RecordInterface::SC_WRITEV,
    .debug = true
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
    .key = RecordInterface::SC_PROC_EXIT
  },
  { // thread exit call replay
    .iminfo = {
      .mod_name = "r3-replay",
      .member_name = "SC_thread_exit"
    },
    .sig = {
      .params = {
        // Error Code
        WASM_TYPE_I32, 
      },
      .results = { } 
    },
    .key = RecordInterface::SC_THREAD_EXIT
  },
  { // thread_spawn call replay
    .iminfo = {
      .mod_name = "wali",
      .member_name = "__wasm_thread_spawn"
    },
    .sig = {
      .params = {
        // Initial setup function pointer
        WASM_TYPE_I32,
        // Arguments to thread
        WASM_TYPE_I32
      },
      .results = { 
        // Child thread ID
        WASM_TYPE_I32
      } 
    },
    .key = RecordInterface::SC_THREAD_SPAWN
  },
  { // futex operation logging to view synchronization points
    .iminfo = {
      .mod_name = "r3-replay",
      .member_name = "SC_futex_log"
    },
    .sig = {
      .params = {
        // Futex Address
        WASM_TYPE_I32,
        // Futex Operation
        WASM_TYPE_I32,
        // Futex Value
        WASM_TYPE_I32,
      },
      .results = {} 
    },
    .key = RecordInterface::SC_FUTEX,
    .debug = true
  },
  { // thread ID tracking
    .iminfo = {
      .mod_name = "r3-replay",
      .member_name = "SC_gettid"
    },
    .sig = {
      .params = {},
      .results = {
        // Thread ID
        WASM_TYPE_I32
      } 
    },
    .key = ReplayInterface::SC_GETTID
  },
  { // debug: logging calls performed by replay
    .iminfo = {
      .mod_name = "r3-replay",
      .member_name = "SC_log_call"
    },
    .sig = {
      .params = {
        // Access Idx
        WASM_TYPE_I32,
        // Func Idx of call target
        WASM_TYPE_I32, 
        // TID
        WASM_TYPE_I32,
        // Prop Arm Idx
        WASM_TYPE_I32,
        // Call ID
        WASM_TYPE_I32,
        // Return value (up to 1)
        WASM_TYPE_I64,
        // Arguments (up to 3)
        WASM_TYPE_I64, WASM_TYPE_I64, WASM_TYPE_I64,
        // Sync ID
        WASM_TYPE_I64
      },
      .results = {} 
    },
    .key = ReplayInterface::SC_LOG_CALL,
    .debug = true
  }
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

/* Add pages to memory statically. Return the old number of pages */
static uint32_t add_pages(MemoryDecl *mem, uint32_t num_pages) {
  wasm_limits_t &memlimit = mem->limits;
  uint32_t memdata_end = memlimit.initial * WASM_PAGE_SIZE;

  uint32_t retval = memlimit.initial;
  memlimit.initial += num_pages;
  if (memlimit.has_max && (memlimit.initial > memlimit.max)) {
    throw std::runtime_error("Not enough memory to instrument");
  }
  return retval;
}

/* Record currently inserts the import functions (call_tracedump & memop_tracedump),
  making the function indices we get at replay time offset by the inserted amount.
  This method performs a transformation from the index for a specific function
  at replay time to that at record time */
static uint32_t transform_to_record_func_idx(uint32_t replay_idx) {
  return replay_idx + NUM_RECORD_IMPORTS - NUM_REPLAY_IMPORTS;
}

/* Mutex Lock/Unlock function creation using the address in Memory[0]
   Returns the function declarations in 'func' */
static void create_mutex_funcs(WasmModule &module, uint32_t mutex_addr, FuncDecl *funcs[2]) {
  MemoryDecl *memory = module.getMemory(0);
  SigDecl sig = {.params= {}, .results = {}};
  SigDecl *void_sig = module.add_sig(sig, false);
  uint32_t void_sig_idx = module.getSigIdx(void_sig);

#define INST(v) InstBasePtr(new v)
  FuncDecl fn1 = {
    .sig = void_sig,
    .pure_locals = wasm_localcsv_t(),
    .num_pure_locals = 0,
    .instructions = {
      INST (BlockInst(void_sig_idx)),
      INST (LoopInst(void_sig_idx)),
      // Try lock
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(0)),
      INST (I32ConstInst(1)),
      INST (I32AtomicRmwCmpxchgInst(2, 0, memory)),
      //
      INST (I32EqzInst()),
      INST (BrIfInst(1)),
      // Wait for notify
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(1)),
      INST (I64ConstInst(-1)),
      INST (MemoryAtomicWait32Inst(2, 0, memory)),
      //
      INST (DropInst()),
      INST (BrInst(0)),

      INST (EndInst()),
      INST (EndInst()),
      INST (EndInst())
    }
  };

  FuncDecl fn2 = {
    .sig = void_sig,
    .pure_locals = wasm_localcsv_t(),
    .num_pure_locals = 0,
    .instructions = {
      // Unlock
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(0)),
      INST (I32AtomicStoreInst(2, 0, memory)),
      // Notify 1 thread max
      INST (I32ConstInst(mutex_addr)),
      INST (I32ConstInst(1)),
      INST (MemoryAtomicNotifyInst(2, 0, memory)),
      //
      INST (DropInst()),
      INST (EndInst())
    }
  };
#undef INST

  funcs[0] = module.add_func(fn1, NULL, "lock_instrument");
  funcs[1] = module.add_func(fn2, NULL, "unlock_instrument");
}


static SigDecl get_sigdecl_from_cdef(CSigDecl csig) {
  SigDecl sig;
  for (int i = 0; csig.params[i] && (i < sizeof(csig.params) / sizeof(csig.params[0])); i++) {
    sig.params.push_back(csig.params[i]);
  }
  for (int i = 0; csig.results[i] && (i < sizeof(csig.results) / sizeof(csig.results[0])); i++) {
    sig.results.push_back(csig.results[i]);
  }
  return sig;
}

#endif