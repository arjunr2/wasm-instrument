#ifndef R3_COMMON_H
#define R3_COMMON_H

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

#endif