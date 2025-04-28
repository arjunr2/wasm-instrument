/** Opcode constants: 172 **/
#define WASM_OP_I32_LOAD 0x28     /* "i32.load" MEMARG */
#define WASM_OP_I64_LOAD 0x29     /* "i64.load", ImmSigs.MEMARG */
#define WASM_OP_F32_LOAD 0x2A     /* "f32.load", ImmSigs.MEMARG */
#define WASM_OP_F64_LOAD 0x2B     /* "f64.load" MEMARG */
#define WASM_OP_I32_LOAD8_S 0x2C  /* "i32.load8_s" MEMARG */
#define WASM_OP_I32_LOAD8_U 0x2D  /* "i32.load8_u" MEMARG */
#define WASM_OP_I32_LOAD16_S 0x2E /* "i32.load16_s" MEMARG */
#define WASM_OP_I32_LOAD16_U 0x2F /* "i32.load16_u" MEMARG */
#define WASM_OP_I64_LOAD8_S 0x30  /* "i64.load8_s", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD8_U 0x31  /* "i64.load8_u", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD16_S 0x32 /* "i64.load16_s", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD16_U 0x33 /* "i64.load16_u", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD32_S 0x34 /* "i64.load32_s", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD32_U 0x35 /* "i64.load32_u", ImmSigs.MEMARG */
#define WASM_OP_I32_STORE 0x36    /* "i32.store" MEMARG */
#define WASM_OP_I64_STORE 0x37    /* "i64.store", ImmSigs.MEMARG */
#define WASM_OP_F32_STORE 0x38    /* "f32.store", ImmSigs.MEMARG */
#define WASM_OP_F64_STORE 0x39    /* "f64.store" MEMARG */
#define WASM_OP_I32_STORE8 0x3A   /* "i32.store8" MEMARG */
#define WASM_OP_I32_STORE16 0x3B  /* "i32.store16" MEMARG */
#define WASM_OP_I64_STORE8 0x3C   /* "i64.store8", ImmSigs.MEMARG */
#define WASM_OP_I64_STORE16 0x3D  /* "i64.store16", ImmSigs.MEMARG */
#define WASM_OP_I64_STORE32 0x3E  /* "i64.store32", ImmSigs.MEMARG */
#define WASM_OP_MEMORY_SIZE 0x3F  /* "memory.size", ImmSigs.MEMORY */
#define WASM_OP_MEMORY_GROW 0x40  /* "memory.grow", ImmSigs.MEMORY */

/** 0xFC extensions: Partially implemented **/
#define WASM_OP_MEMORY_INIT 0xFC08 /* "memory.init", ImmSigs.DATA_MEMORY */
#define WASM_OP_MEMORY_COPY 0xFC0A /* "memory.copy", ImmSigs.MEMORYCP */
#define WASM_OP_MEMORY_FILL 0xFC0B /* "memory.fill", ImmSigs.MEMORY */

/** Threads/Atomics: 0xFE extensions **/
#define WASM_OP_MEMORY_ATOMIC_NOTIFY                                           \
    0xFE00 /* "memory.atomic.notify", ImmSigs.MEMARG */
#define WASM_OP_MEMORY_ATOMIC_WAIT32                                           \
    0xFE01 /* "memory.atomic.wait32", ImmSigs.MEMARG */
#define WASM_OP_MEMORY_ATOMIC_WAIT64                                           \
    0xFE02                          /* "memory.atomic.wait64", ImmSigs.MEMARG */
#define WASM_OP_ATOMIC_FENCE 0xFE03 /* "atomic.fence", ImmSigs.NONE */

#define WASM_OP_I32_ATOMIC_LOAD 0xFE10 /* "i32.atomic.load", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD 0xFE11 /* "i64.atomic.load", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_LOAD8_U                                             \
    0xFE12 /* "i32.atomic.load8_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_LOAD16_U                                            \
    0xFE13 /* "i32.atomic.load16_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD8_U                                             \
    0xFE14 /* "i64.atomic.load8_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD16_U                                            \
    0xFE15 /* "i64.atomic.load16_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD32_U                                            \
    0xFE16 /* "i64.atomic.load32_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_STORE 0xFE17 /* "i32.atomic.store", ImmSigs.MEMARG  \
                                         */
#define WASM_OP_I64_ATOMIC_STORE 0xFE18 /* "i64.atomic.store", ImmSigs.MEMARG  \
                                         */
#define WASM_OP_I32_ATOMIC_STORE8                                              \
    0xFE19 /* "i32.atomic.store8", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_STORE16                                             \
    0xFE1A /* "i32.atomic.store16", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_STORE8                                              \
    0xFE1B /* "i64.atomic.store8", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_STORE16                                             \
    0xFE1C /* "i64.atomic.store16", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_STORE32                                             \
    0xFE1D /* "i64.atomic.store32", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_ADD                                             \
    0xFE1E /* "i32.atomic.rmw.add", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_ADD                                             \
    0xFE1F /* "i64.atomic.rmw.add", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_ADD_U                                          \
    0xFE20 /* "i32.atomic.rmw8.add_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_ADD_U                                         \
    0xFE21 /* "i32.atomic.rmw16.add_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_ADD_U                                          \
    0xFE22 /* "i64.atomic.rmw8.add_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_ADD_U                                         \
    0xFE23 /* "i64.atomic.rmw16.add_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_ADD_U                                         \
    0xFE24 /* "i64.atomic.rmw32.add_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_SUB                                             \
    0xFE25 /* "i32.atomic.rmw.sub", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_SUB                                             \
    0xFE26 /* "i64.atomic.rmw.sub", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_SUB_U                                          \
    0xFE27 /* "i32.atomic.rmw8.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_SUB_U                                         \
    0xFE28 /* "i32.atomic.rmw16.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_SUB_U                                          \
    0xFE29 /* "i64.atomic.rmw8.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_SUB_U                                         \
    0xFE2A /* "i64.atomic.rmw16.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_SUB_U                                         \
    0xFE2B /* "i64.atomic.rmw32.sub_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_AND                                             \
    0xFE2C /* "i32.atomic.rmw.and", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_AND                                             \
    0xFE2D /* "i64.atomic.rmw.and", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_AND_U                                          \
    0xFE2E /* "i32.atomic.rmw8.and_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_AND_U                                         \
    0xFE2F /* "i32.atomic.rmw16.and_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_AND_U                                          \
    0xFE30 /* "i64.atomic.rmw8.and_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_AND_U                                         \
    0xFE31 /* "i64.atomic.rmw16.and_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_AND_U                                         \
    0xFE32 /* "i64.atomic.rmw32.and_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_OR                                              \
    0xFE33 /* "i32.atomic.rmw.or", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_OR                                              \
    0xFE34 /* "i64.atomic.rmw.or", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_OR_U                                           \
    0xFE35 /* "i32.atomic.rmw8.or_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_OR_U                                          \
    0xFE36 /* "i32.atomic.rmw16.or_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_OR_U                                           \
    0xFE37 /* "i64.atomic.rmw8.or_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_OR_U                                          \
    0xFE38 /* "i64.atomic.rmw16.or_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_OR_U                                          \
    0xFE39 /* "i64.atomic.rmw32.or_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_XOR                                             \
    0xFE3A /* "i32.atomic.rmw.xor", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_XOR                                             \
    0xFE3B /* "i64.atomic.rmw.xor", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_XOR_U                                          \
    0xFE3C /* "i32.atomic.rmw8.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_XOR_U                                         \
    0xFE3D /* "i32.atomic.rmw16.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_XOR_U                                          \
    0xFE3E /* "i64.atomic.rmw8.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_XOR_U                                         \
    0xFE3F /* "i64.atomic.rmw16.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_XOR_U                                         \
    0xFE40 /* "i64.atomic.rmw32.xor_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_XCHG                                            \
    0xFE41 /* "i32.atomic.rmw.xchg", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_XCHG                                            \
    0xFE42 /* "i64.atomic.rmw.xchg", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_XCHG_U                                         \
    0xFE43 /* "i32.atomic.rmw8.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_XCHG_U                                        \
    0xFE44 /* "i32.atomic.rmw16.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_XCHG_U                                         \
    0xFE45 /* "i64.atomic.rmw8.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_XCHG_U                                        \
    0xFE46 /* "i64.atomic.rmw16.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_XCHG_U                                        \
    0xFE47 /* "i64.atomic.rmw32.xchg_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_CMPXCHG                                         \
    0xFE48 /* "i32.atomic.rmw.cmpxchg", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_CMPXCHG                                         \
    0xFE49 /* "i64.atomic.rmw.cmpxchg", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_CMPXCHG_U                                      \
    0xFE4A /* "i32.atomic.rmw8.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_CMPXCHG_U                                     \
    0xFE4B /* "i32.atomic.rmw16.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_CMPXCHG_U                                      \
    0xFE4C /* "i64.atomic.rmw8.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_CMPXCHG_U                                     \
    0xFE4D /* "i64.atomic.rmw16.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_CMPXCHG_U                                     \
    0xFE4E /* "i64.atomic.rmw32.cmpxchg_u", ImmSigs.MEMARG */
