#pragma once
/* Must be a C-compatible header for designated init */

/* Opcode constants: 172 */
#define WASM_OP_UNREACHABLE		0x00 /* "unreachable" */
#define WASM_OP_NOP			0x01 /* "nop" */
#define WASM_OP_BLOCK			0x02 /* "block" BLOCKT */
#define WASM_OP_LOOP			0x03 /* "loop" BLOCKT */
#define WASM_OP_IF			0x04 /* "if" BLOCKT */
#define WASM_OP_ELSE			0x05 /* "else" */
#define WASM_OP_END			0x0B /* "end" */
#define WASM_OP_BR			0x0C /* "br" LABEL */
#define WASM_OP_BR_IF			0x0D /* "br_if" LABEL */
#define WASM_OP_BR_TABLE		0x0E /* "br_table" LABELS */
#define WASM_OP_RETURN			0x0F /* "return" */
#define WASM_OP_CALL			0x10 /* "call" FUNC */
#define WASM_OP_CALL_INDIRECT		0x11 /* "call_indirect" SIG_TABLE */
#define WASM_OP_DROP			0x1A /* "drop" */
#define WASM_OP_SELECT			0x1B /* "select" */
#define WASM_OP_LOCAL_GET		0x20 /* "local.get" LOCAL */
#define WASM_OP_LOCAL_SET		0x21 /* "local.set" LOCAL */
#define WASM_OP_LOCAL_TEE		0x22 /* "local.tee" LOCAL */
#define WASM_OP_GLOBAL_GET		0x23 /* "global.get" GLOBAL */
#define WASM_OP_GLOBAL_SET		0x24 /* "global.set" GLOBAL */
#define WASM_OP_I32_LOAD		0x28 /* "i32.load" MEMARG */
#define WASM_OP_I64_LOAD		0x29 /* "i64.load", ImmSigs.MEMARG */
#define WASM_OP_F32_LOAD		0x2A /* "f32.load", ImmSigs.MEMARG */
#define WASM_OP_F64_LOAD		0x2B /* "f64.load" MEMARG */
#define WASM_OP_I32_LOAD8_S		0x2C /* "i32.load8_s" MEMARG */
#define WASM_OP_I32_LOAD8_U		0x2D /* "i32.load8_u" MEMARG */
#define WASM_OP_I32_LOAD16_S		0x2E /* "i32.load16_s" MEMARG */
#define WASM_OP_I32_LOAD16_U		0x2F /* "i32.load16_u" MEMARG */
#define WASM_OP_I64_LOAD8_S		0x30 /* "i64.load8_s", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD8_U		0x31 /* "i64.load8_u", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD16_S		0x32 /* "i64.load16_s", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD16_U		0x33 /* "i64.load16_u", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD32_S		0x34 /* "i64.load32_s", ImmSigs.MEMARG */
#define WASM_OP_I64_LOAD32_U		0x35 /* "i64.load32_u", ImmSigs.MEMARG */
#define WASM_OP_I32_STORE		0x36 /* "i32.store" MEMARG */
#define WASM_OP_I64_STORE		0x37 /* "i64.store", ImmSigs.MEMARG */
#define WASM_OP_F32_STORE		0x38 /* "f32.store", ImmSigs.MEMARG */
#define WASM_OP_F64_STORE		0x39 /* "f64.store" MEMARG */
#define WASM_OP_I32_STORE8		0x3A /* "i32.store8" MEMARG */
#define WASM_OP_I32_STORE16		0x3B /* "i32.store16" MEMARG */
#define WASM_OP_I64_STORE8		0x3C /* "i64.store8", ImmSigs.MEMARG */
#define WASM_OP_I64_STORE16		0x3D /* "i64.store16", ImmSigs.MEMARG */
#define WASM_OP_I64_STORE32		0x3E /* "i64.store32", ImmSigs.MEMARG */
#define WASM_OP_MEMORY_SIZE		0x3F /* "memory.size", ImmSigs.MEMORY */
#define WASM_OP_MEMORY_GROW		0x40 /* "memory.grow", ImmSigs.MEMORY */
#define WASM_OP_I32_CONST		0x41 /* "i32.const" I32 */
#define WASM_OP_I64_CONST		0x42 /* "i64.const", ImmSigs.I64 */
#define WASM_OP_F32_CONST		0x43 /* "f32.const", ImmSigs.F32 */
#define WASM_OP_F64_CONST		0x44 /* "f64.const" F64 */
#define WASM_OP_I32_EQZ			0x45 /* "i32.eqz" */
#define WASM_OP_I32_EQ			0x46 /* "i32.eq" */
#define WASM_OP_I32_NE			0x47 /* "i32.ne" */
#define WASM_OP_I32_LT_S		0x48 /* "i32.lt_s" */
#define WASM_OP_I32_LT_U		0x49 /* "i32.lt_u" */
#define WASM_OP_I32_GT_S		0x4A /* "i32.gt_s" */
#define WASM_OP_I32_GT_U		0x4B /* "i32.gt_u" */
#define WASM_OP_I32_LE_S		0x4C /* "i32.le_s" */
#define WASM_OP_I32_LE_U		0x4D /* "i32.le_u" */
#define WASM_OP_I32_GE_S		0x4E /* "i32.ge_s" */
#define WASM_OP_I32_GE_U		0x4F /* "i32.ge_u" */
#define WASM_OP_I64_EQZ			0x50 /* "i64.eqz", ImmSigs.NONE */
#define WASM_OP_I64_EQ			0x51 /* "i64.eq", ImmSigs.NONE */
#define WASM_OP_I64_NE			0x52 /* "i64.ne", ImmSigs.NONE */
#define WASM_OP_I64_LT_S		0x53 /* "i64.lt_s", ImmSigs.NONE */
#define WASM_OP_I64_LT_U		0x54 /* "i64.lt_u", ImmSigs.NONE */
#define WASM_OP_I64_GT_S		0x55 /* "i64.gt_s", ImmSigs.NONE */
#define WASM_OP_I64_GT_U		0x56 /* "i64.gt_u", ImmSigs.NONE */
#define WASM_OP_I64_LE_S		0x57 /* "i64.le_s", ImmSigs.NONE */
#define WASM_OP_I64_LE_U		0x58 /* "i64.le_u", ImmSigs.NONE */
#define WASM_OP_I64_GE_S		0x59 /* "i64.ge_s", ImmSigs.NONE */
#define WASM_OP_I64_GE_U		0x5A /* "i64.ge_u", ImmSigs.NONE */
#define WASM_OP_F32_EQ			0x5B /* "f32.eq", ImmSigs.NONE */
#define WASM_OP_F32_NE			0x5C /* "f32.ne", ImmSigs.NONE */
#define WASM_OP_F32_LT			0x5D /* "f32.lt", ImmSigs.NONE */
#define WASM_OP_F32_GT			0x5E /* "f32.gt", ImmSigs.NONE */
#define WASM_OP_F32_LE			0x5F /* "f32.le", ImmSigs.NONE */
#define WASM_OP_F32_GE			0x60 /* "f32.ge", ImmSigs.NONE */
#define WASM_OP_F64_EQ			0x61 /* "f64.eq" */
#define WASM_OP_F64_NE			0x62 /* "f64.ne" */
#define WASM_OP_F64_LT			0x63 /* "f64.lt" */
#define WASM_OP_F64_GT			0x64 /* "f64.gt" */
#define WASM_OP_F64_LE			0x65 /* "f64.le" */
#define WASM_OP_F64_GE			0x66 /* "f64.ge" */
#define WASM_OP_I32_CLZ			0x67 /* "i32.clz" */
#define WASM_OP_I32_CTZ			0x68 /* "i32.ctz" */
#define WASM_OP_I32_POPCNT		0x69 /* "i32.popcnt" */
#define WASM_OP_I32_ADD			0x6A /* "i32.add" */
#define WASM_OP_I32_SUB			0x6B /* "i32.sub" */
#define WASM_OP_I32_MUL			0x6C /* "i32.mul" */
#define WASM_OP_I32_DIV_S		0x6D /* "i32.div_s" */
#define WASM_OP_I32_DIV_U		0x6E /* "i32.div_u" */
#define WASM_OP_I32_REM_S		0x6F /* "i32.rem_s" */
#define WASM_OP_I32_REM_U		0x70 /* "i32.rem_u" */
#define WASM_OP_I32_AND			0x71 /* "i32.and" */
#define WASM_OP_I32_OR			0x72 /* "i32.or" */
#define WASM_OP_I32_XOR			0x73 /* "i32.xor" */
#define WASM_OP_I32_SHL			0x74 /* "i32.shl" */
#define WASM_OP_I32_SHR_S		0x75 /* "i32.shr_s" */
#define WASM_OP_I32_SHR_U		0x76 /* "i32.shr_u" */
#define WASM_OP_I32_ROTL		0x77 /* "i32.rotl" */
#define WASM_OP_I32_ROTR		0x78 /* "i32.rotr" */
#define WASM_OP_I64_CLZ			0x79 /* "i64.clz", ImmSigs.NONE */
#define WASM_OP_I64_CTZ			0x7A /* "i64.ctz", ImmSigs.NONE */
#define WASM_OP_I64_POPCNT		0x7B /* "i64.popcnt", ImmSigs.NONE */
#define WASM_OP_I64_ADD			0x7C /* "i64.add", ImmSigs.NONE */
#define WASM_OP_I64_SUB			0x7D /* "i64.sub", ImmSigs.NONE */
#define WASM_OP_I64_MUL			0x7E /* "i64.mul", ImmSigs.NONE */
#define WASM_OP_I64_DIV_S		0x7F /* "i64.div_s", ImmSigs.NONE */
#define WASM_OP_I64_DIV_U		0x80 /* "i64.div_u", ImmSigs.NONE */
#define WASM_OP_I64_REM_S		0x81 /* "i64.rem_s", ImmSigs.NONE */
#define WASM_OP_I64_REM_U		0x82 /* "i64.rem_u", ImmSigs.NONE */
#define WASM_OP_I64_AND			0x83 /* "i64.and", ImmSigs.NONE */
#define WASM_OP_I64_OR			0x84 /* "i64.or", ImmSigs.NONE */
#define WASM_OP_I64_XOR			0x85 /* "i64.xor", ImmSigs.NONE */
#define WASM_OP_I64_SHL			0x86 /* "i64.shl", ImmSigs.NONE */
#define WASM_OP_I64_SHR_S		0x87 /* "i64.shr_s", ImmSigs.NONE */
#define WASM_OP_I64_SHR_U		0x88 /* "i64.shr_u", ImmSigs.NONE */
#define WASM_OP_I64_ROTL		0x89 /* "i64.rotl", ImmSigs.NONE */
#define WASM_OP_I64_ROTR		0x8A /* "i64.rotr", ImmSigs.NONE */
#define WASM_OP_F32_ABS			0x8B /* "f32.abs", ImmSigs.NONE */
#define WASM_OP_F32_NEG			0x8C /* "f32.neg", ImmSigs.NONE */
#define WASM_OP_F32_CEIL		0x8D /* "f32.ceil", ImmSigs.NONE */
#define WASM_OP_F32_FLOOR		0x8E /* "f32.floor", ImmSigs.NONE */
#define WASM_OP_F32_TRUNC		0x8F /* "f32.trunc", ImmSigs.NONE */
#define WASM_OP_F32_NEAREST		0x90 /* "f32.nearest", ImmSigs.NONE */
#define WASM_OP_F32_SQRT		0x91 /* "f32.sqrt", ImmSigs.NONE */
#define WASM_OP_F32_ADD			0x92 /* "f32.add", ImmSigs.NONE */
#define WASM_OP_F32_SUB			0x93 /* "f32.sub", ImmSigs.NONE */
#define WASM_OP_F32_MUL			0x94 /* "f32.mul", ImmSigs.NONE */
#define WASM_OP_F32_DIV			0x95 /* "f32.div", ImmSigs.NONE */
#define WASM_OP_F32_MIN			0x96 /* "f32.min", ImmSigs.NONE */
#define WASM_OP_F32_MAX			0x97 /* "f32.max", ImmSigs.NONE */
#define WASM_OP_F32_COPYSIGN		0x98 /* "f32.copysign", ImmSigs.NONE */
#define WASM_OP_F64_ABS			0x99 /* "f64.abs", ImmSigs.NONE */
#define WASM_OP_F64_NEG			0x9A /* "f64.neg", ImmSigs.NONE */
#define WASM_OP_F64_CEIL		0x9B /* "f64.ceil", ImmSigs.NONE */
#define WASM_OP_F64_FLOOR		0x9C /* "f64.floor", ImmSigs.NONE */
#define WASM_OP_F64_TRUNC		0x9D /* "f64.trunc", ImmSigs.NONE */
#define WASM_OP_F64_NEAREST		0x9E /* "f64.nearest", ImmSigs.NONE */
#define WASM_OP_F64_SQRT		0x9F /* "f64.sqrt", ImmSigs.NONE */
#define WASM_OP_F64_ADD			0xA0 /* "f64.add" */
#define WASM_OP_F64_SUB			0xA1 /* "f64.sub" */
#define WASM_OP_F64_MUL			0xA2 /* "f64.mul" */
#define WASM_OP_F64_DIV			0xA3 /* "f64.div" */
#define WASM_OP_F64_MIN			0xA4 /* "f64.min", ImmSigs.NONE */
#define WASM_OP_F64_MAX			0xA5 /* "f64.max", ImmSigs.NONE */
#define WASM_OP_F64_COPYSIGN		0xA6 /* "f64.copysign", ImmSigs.NONE */
#define WASM_OP_I32_WRAP_I64		0xA7 /* "i32.wrap_i64", ImmSigs.NONE */
#define WASM_OP_I32_TRUNC_F32_S		0xA8 /* "i32.trunc_f32_s", ImmSigs.NONE */
#define WASM_OP_I32_TRUNC_F32_U		0xA9 /* "i32.trunc_f32_u", ImmSigs.NONE */
#define WASM_OP_I32_TRUNC_F64_S		0xAA /* "i32.trunc_f64_s" */
#define WASM_OP_I32_TRUNC_F64_U		0xAB /* "i32.trunc_f64_u" */
#define WASM_OP_I64_EXTEND_I32_S	0xAC /* "i64.extend_i32_s", ImmSigs.NONE */
#define WASM_OP_I64_EXTEND_I32_U	0xAD /* "i64.extend_i32_u", ImmSigs.NONE */
#define WASM_OP_I64_TRUNC_F32_S		0xAE /* "i64.trunc_f32_s", ImmSigs.NONE */
#define WASM_OP_I64_TRUNC_F32_U		0xAF /* "i64.trunc_f32_u", ImmSigs.NONE */
#define WASM_OP_I64_TRUNC_F64_S		0xB0 /* "i64.trunc_f64_s", ImmSigs.NONE */
#define WASM_OP_I64_TRUNC_F64_U		0xB1 /* "i64.trunc_f64_u", ImmSigs.NONE */
#define WASM_OP_F32_CONVERT_I32_S	0xB2 /* "f32.convert_i32_s", ImmSigs.NONE */
#define WASM_OP_F32_CONVERT_I32_U	0xB3 /* "f32.convert_i32_u", ImmSigs.NONE */
#define WASM_OP_F32_CONVERT_I64_S	0xB4 /* "f32.convert_i64_s", ImmSigs.NONE */
#define WASM_OP_F32_CONVERT_I64_U	0xB5 /* "f32.convert_i64_u", ImmSigs.NONE */
#define WASM_OP_F32_DEMOTE_F64		0xB6 /* "f32.demote_f64", ImmSigs.NONE */
#define WASM_OP_F64_CONVERT_I32_S	0xB7 /* "f64.convert_i32_s" */
#define WASM_OP_F64_CONVERT_I32_U	0xB8 /* "f64.convert_i32_u" */
#define WASM_OP_F64_CONVERT_I64_S	0xB9 /* "f64.convert_i64_s" */
#define WASM_OP_F64_CONVERT_I64_U	0xBA /* "f64.convert_i64_u" */
#define WASM_OP_F64_PROMOTE_F32		0xBB /* "f64.promote_f32", ImmSigs.NONE */
#define WASM_OP_I32_REINTERPRET_F32	0xBC /* "i32.reinterpret_f32", ImmSigs.NONE */
#define WASM_OP_I64_REINTERPRET_F64	0xBD /* "i64.reinterpret_f64", ImmSigs.NONE */
#define WASM_OP_F32_REINTERPRET_I32	0xBE /* "f32.reinterpret_i32", ImmSigs.NONE */
#define WASM_OP_F64_REINTERPRET_I64	0xBF /* "f64.reinterpret_i64", ImmSigs.NONE */

/* Illegal opcodes: 25 */
#define WASM_OP_TRY			0x06 /* "try", ImmSigs.BLOCKT */
#define WASM_OP_CATCH			0x07 /* "catch", ImmSigs.TAG */
#define WASM_OP_THROW			0x08 /* "throw", ImmSigs.TAG */
#define WASM_OP_RETHROW			0x09 /* "rethrow", ImmSigs.NONE */
#define WASM_OP_RETURN_CALL		0x12 /* "return_call", ImmSigs.FUNC */
#define WASM_OP_RETURN_CALL_INDIRECT	0x13 /* "return_call_indirect", ImmSigs.SIG_TABLE */
#define WASM_OP_CALL_REF		0x14 /* "call_ref", ImmSigs.NONE */
#define WASM_OP_RETURN_CALL_REF		0x15 /* "return_call_ref", ImmSigs.NONE */
#define WASM_OP_DELEGATE		0x18 /* "delegate", ImmSigs.NONE */
#define WASM_OP_CATCH_ALL		0x19 /* "catch_all", ImmSigs.NONE */
#define WASM_OP_SELECT_T		0x1C /* "select", ImmSigs.VALTS */
#define WASM_OP_TABLE_GET		0x25 /* "table.get", ImmSigs.TABLE */
#define WASM_OP_TABLE_SET		0x26 /* "table.set", ImmSigs.TABLE */
#define WASM_OP_I32_EXTEND8_S		0xC0 /* "i32.extend8_s", ImmSigs.NONE */
#define WASM_OP_I32_EXTEND16_S		0xC1 /* "i32.extend16_s", ImmSigs.NONE */
#define WASM_OP_I64_EXTEND8_S		0xC2 /* "i64.extend8_s", ImmSigs.NONE */
#define WASM_OP_I64_EXTEND16_S		0xC3 /* "i64.extend16_s", ImmSigs.NONE */
#define WASM_OP_I64_EXTEND32_S		0xC4 /* "i64.extend32_s", ImmSigs.NONE */
#define WASM_OP_REF_NULL		0xD0 /* "ref.null", ImmSigs.REFNULLT */
#define WASM_OP_REF_IS_NULL		0xD1 /* "ref.is_null", ImmSigs.NONE */
#define WASM_OP_REF_FUNC		0xD2 /* "ref.func", ImmSigs.FUNC */
#define WASM_OP_REF_AS_NON_NULL		0xD3 /* "ref.as_non_null", ImmSigs.NONE */
#define WASM_OP_BR_ON_NULL		0xD4 /* "br_on_null", ImmSigs.LABEL */
#define WASM_OP_REF_EQ			0xD5 /* "ref.eq", ImmSigs.NONE */
#define WASM_OP_BR_ON_NON_NULL		0xD6 /* "br_on_non_null", ImmSigs.LABEL */



/* Multibyte opcode classes */
#define WASM_EXT1_GCREF 0xFB
#define WASM_EXT1_FC 0xFC
#define WASM_EXT1_SIMD 0xFD
#define WASM_EXT1_THREADS  0xFE

/* 0xFC extensions: Partially implemented */
#define WASM_OP_MEMORY_INIT 0xFC08 /* "memory.init", ImmSigs.DATA_MEMORY */
#define WASM_OP_DATA_DROP 0xFC09 /* "data.drop", ImmSigs.DATA */
#define WASM_OP_MEMORY_COPY 0xFC0A /* "memory.copy", ImmSigs.MEMORY_CP */
#define WASM_OP_MEMORY_FILL 0xFC0B /* "memory.fill", ImmSigs.MEMORY */

/* Threads/Atomics: 0xFE extensions */
#define WASM_OP_MEMORY_ATOMIC_NOTIFY 0xFE00 /* "memory.atomic.notify", ImmSigs.MEMARG */
#define WASM_OP_MEMORY_ATOMIC_WAIT32 0xFE01 /* "memory.atomic.wait32", ImmSigs.MEMARG */
#define WASM_OP_MEMORY_ATOMIC_WAIT64 0xFE02 /* "memory.atomic.wait64", ImmSigs.MEMARG */
#define WASM_OP_ATOMIC_FENCE 0xFE03 /* "atomic.fence", ImmSigs.NONE */

#define WASM_OP_I32_ATOMIC_LOAD 0xFE10 /* "i32.atomic.load", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD 0xFE11 /* "i64.atomic.load", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_LOAD8_U 0xFE12 /* "i32.atomic.load8_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_LOAD16_U 0xFE13 /* "i32.atomic.load16_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD8_U 0xFE14 /* "i64.atomic.load8_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD16_U 0xFE15 /* "i64.atomic.load16_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_LOAD32_U 0xFE16 /* "i64.atomic.load32_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_STORE 0xFE17 /* "i32.atomic.store", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_STORE 0xFE18 /* "i64.atomic.store", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_STORE8 0xFE19 /* "i32.atomic.store8", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_STORE16 0xFE1A /* "i32.atomic.store16", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_STORE8 0xFE1B /* "i64.atomic.store8", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_STORE16 0xFE1C /* "i64.atomic.store16", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_STORE32 0xFE1D /* "i64.atomic.store32", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_ADD 0xFE1E /* "i32.atomic.rmw.add", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_ADD 0xFE1F /* "i64.atomic.rmw.add", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_ADD_U 0xFE20 /* "i32.atomic.rmw8.add_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_ADD_U 0xFE21 /* "i32.atomic.rmw16.add_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_ADD_U 0xFE22 /* "i64.atomic.rmw8.add_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_ADD_U 0xFE23 /* "i64.atomic.rmw16.add_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_ADD_U 0xFE24 /* "i64.atomic.rmw32.add_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_SUB 0xFE25 /* "i32.atomic.rmw.sub", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_SUB 0xFE26 /* "i64.atomic.rmw.sub", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_SUB_U 0xFE27 /* "i32.atomic.rmw8.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_SUB_U 0xFE28 /* "i32.atomic.rmw16.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_SUB_U 0xFE29 /* "i64.atomic.rmw8.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_SUB_U 0xFE2A /* "i64.atomic.rmw16.sub_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_SUB_U 0xFE2B /* "i64.atomic.rmw32.sub_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_AND 0xFE2C /* "i32.atomic.rmw.and", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_AND 0xFE2D /* "i64.atomic.rmw.and", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_AND_U 0xFE2E /* "i32.atomic.rmw8.and_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_AND_U 0xFE2F /* "i32.atomic.rmw16.and_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_AND_U 0xFE30 /* "i64.atomic.rmw8.and_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_AND_U 0xFE31 /* "i64.atomic.rmw16.and_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_AND_U 0xFE32 /* "i64.atomic.rmw32.and_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_OR 0xFE33 /* "i32.atomic.rmw.or", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_OR 0xFE34 /* "i64.atomic.rmw.or", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_OR_U 0xFE35 /* "i32.atomic.rmw8.or_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_OR_U 0xFE36 /* "i32.atomic.rmw16.or_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_OR_U 0xFE37 /* "i64.atomic.rmw8.or_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_OR_U 0xFE38 /* "i64.atomic.rmw16.or_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_OR_U 0xFE39 /* "i64.atomic.rmw32.or_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_XOR 0xFE3A /* "i32.atomic.rmw.xor", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_XOR 0xFE3B /* "i64.atomic.rmw.xor", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_XOR_U 0xFE3C /* "i32.atomic.rmw8.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_XOR_U 0xFE3D /* "i32.atomic.rmw16.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_XOR_U 0xFE3E /* "i64.atomic.rmw8.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_XOR_U 0xFE3F /* "i64.atomic.rmw16.xor_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_XOR_U 0xFE40 /* "i64.atomic.rmw32.xor_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_XCHG 0xFE41 /* "i32.atomic.rmw.xchg", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_XCHG 0xFE42 /* "i64.atomic.rmw.xchg", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_XCHG_U 0xFE43 /* "i32.atomic.rmw8.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_XCHG_U 0xFE44 /* "i32.atomic.rmw16.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_XCHG_U 0xFE45 /* "i64.atomic.rmw8.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_XCHG_U 0xFE46 /* "i64.atomic.rmw16.xchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_XCHG_U 0xFE47 /* "i64.atomic.rmw32.xchg_u", ImmSigs.MEMARG */

#define WASM_OP_I32_ATOMIC_RMW_CMPXCHG 0xFE48 /* "i32.atomic.rmw.cmpxchg", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW_CMPXCHG 0xFE49 /* "i64.atomic.rmw.cmpxchg", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW8_CMPXCHG_U 0xFE4A /* "i32.atomic.rmw8.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I32_ATOMIC_RMW16_CMPXCHG_U 0xFE4B /* "i32.atomic.rmw16.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW8_CMPXCHG_U 0xFE4C /* "i64.atomic.rmw8.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW16_CMPXCHG_U 0xFE4D /* "i64.atomic.rmw16.cmpxchg_u", ImmSigs.MEMARG */
#define WASM_OP_I64_ATOMIC_RMW32_CMPXCHG_U 0xFE4E /* "i64.atomic.rmw32.cmpxchg_u", ImmSigs.MEMARG */


