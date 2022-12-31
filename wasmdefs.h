#pragma once

#define WASM_MAGIC 0x6d736100u
#define WASM_VERSION 1

#define PAGE_SIZE 65536

// Section constants
#define WASM_SECT_CUSTOM 0
#define WASM_SECT_TYPE 1
#define WASM_SECT_IMPORT 2
#define WASM_SECT_FUNCTION 3
#define WASM_SECT_TABLE 4
#define WASM_SECT_MEMORY 5
#define WASM_SECT_GLOBAL 6
#define WASM_SECT_EXPORT 7
#define WASM_SECT_START 8
#define WASM_SECT_ELEMENT 9
#define WASM_SECT_CODE 10
#define WASM_SECT_DATA 11
#define WASM_SECT_DATACOUNT 12

// Type decoding
#define WASM_TYPE_I32 0x7F
#define WASM_TYPE_I64 0x7E
#define WASM_TYPE_F32 0x7D
#define WASM_TYPE_F64 0x7C
#define WASM_TYPE_V128 0x7B 
#define WASM_TYPE_FUNCREF 0x70
#define WASM_TYPE_EXTERNREF 0x6F

// Mutable val
#define WASM_TYPE_GLOBAL_MUTABLE 0x01

// Kind decoding
#define WASM_KIND_FUNC 0x60

// Import/export desc decoding
#define WASM_IE_DESC_FUNC 0x00
#define WASM_IE_DESC_TABLE 0x01
#define WASM_IE_DESC_MEM 0x02
#define WASM_IE_DESC_GLOBAL 0x03




// Opcode constants
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
#define WASM_OP_RETURN_CALL		0x12 /* "return_call" FUNC */
#define WASM_OP_RETURN_CALL_INDIRECT		0x13 /* "return_call_indirect" SIG_TABLE */
#define WASM_OP_CALL_INDIRECT		0x11 /* "call_indirect" SIG_TABLE */

#define WASM_OP_DROP			0x1A /* "drop" */
#define WASM_OP_SELECT			0x1B /* "select" */

#define WASM_OP_LOCAL_GET		0x20 /* "local.get" LOCAL */
#define WASM_OP_LOCAL_SET		0x21 /* "local.set" LOCAL */
#define WASM_OP_LOCAL_TEE		0x22 /* "local.tee" LOCAL */
#define WASM_OP_GLOBAL_GET		0x23 /* "global.get" GLOBAL */
#define WASM_OP_GLOBAL_SET		0x24 /* "global.set" GLOBAL */
#define WASM_OP_I32_LOAD		0x28 /* "i32.load" MEMARG */
#define WASM_OP_I64_LOAD		0x29 /* "i64.load" MEMARG */
#define WASM_OP_F32_LOAD		0x2A /* "f32.load" MEMARG */
#define WASM_OP_F64_LOAD		0x2B /* "f64.load" MEMARG */
#define WASM_OP_I32_LOAD8_S		0x2C /* "i32.load8_s" MEMARG */
#define WASM_OP_I32_LOAD8_U		0x2D /* "i32.load8_u" MEMARG */
#define WASM_OP_I32_LOAD16_S		0x2E /* "i32.load16_s" MEMARG */
#define WASM_OP_I32_LOAD16_U		0x2F /* "i32.load16_u" MEMARG */


#define WASM_OP_I64_LOAD8_S		0x30 /* "i64.load8_s" MEMARG */
#define WASM_OP_I64_LOAD8_U		0x31 /* "i64.load8_u" MEMARG */
#define WASM_OP_I64_LOAD16_S		0x32 /* "i64.load16_s" MEMARG */
#define WASM_OP_I64_LOAD16_U		0x33 /* "i64.load16_u" MEMARG */
#define WASM_OP_I64_LOAD32_S		0x34 /* "i64.load32_s" MEMARG */
#define WASM_OP_I64_LOAD32_U		0x35 /* "i64.load32_u" MEMARG */
#define WASM_OP_I32_STORE		0x36 /* "i32.store" MEMARG */
#define WASM_OP_I64_STORE		0x37 /* "i64.store" MEMARG */
#define WASM_OP_F32_STORE		0x38 /* "f32.store" MEMARG */
#define WASM_OP_F64_STORE		0x39 /* "f64.store" MEMARG */
#define WASM_OP_I32_STORE8		0x3A /* "i32.store8" MEMARG */
#define WASM_OP_I32_STORE16		0x3B /* "i32.store16" MEMARG */
#define WASM_OP_I64_STORE8		0x3C /* "i64.store8" MEMARG */
#define WASM_OP_I64_STORE16		0x3D /* "i64.store16" MEMARG */
#define WASM_OP_I64_STORE32		0x3E /* "i64.store32" MEMARG */
#define WASM_OP_MEMORY_SIZE   0x3F /* "memory.size" */

#define WASM_OP_MEMORY_GROW 0x40 /* "memory.grow" */
#define WASM_OP_I32_CONST		0x41 /* "i32.const" I32 */
#define WASM_OP_I64_CONST		0x42 /* "i64.const" I64 */
#define WASM_OP_F32_CONST		0x43 /* "f32.const" F32 */
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

#define WASM_OP_I64_EQZ			0x50 /* "i64.eqz" */
#define WASM_OP_I64_EQ			0x51 /* "i64.eq" */
#define WASM_OP_I64_NE			0x52 /* "i64.ne" */
#define WASM_OP_I64_LT_S		0x53 /* "i64.lt_s" */
#define WASM_OP_I64_LT_U		0x54 /* "i64.lt_u" */
#define WASM_OP_I64_GT_S		0x55 /* "i64.gt_s" */
#define WASM_OP_I64_GT_U		0x56 /* "i64.gt_u" */
#define WASM_OP_I64_LE_S		0x57 /* "i64.le_s" */
#define WASM_OP_I64_LE_U		0x58 /* "i64.le_u" */
#define WASM_OP_I64_GE_S		0x59 /* "i64.ge_s" */
#define WASM_OP_I64_GE_U		0x5A /* "i64.ge_u" */
#define WASM_OP_F32_EQ			0x5B /* "f32.eq" */
#define WASM_OP_F32_NE			0x5C /* "f32.ne" */
#define WASM_OP_F32_LT			0x5D /* "f32.lt" */
#define WASM_OP_F32_GT			0x5E /* "f32.gt" */
#define WASM_OP_F32_LE			0x5F /* "f32.le" */
#define WASM_OP_F32_GE			0x60 /* "f32.ge" */

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
#define WASM_OP_I64_CLZ			0x79 /* "i64.clz" */
#define WASM_OP_I64_CTZ			0x7A /* "i64.ctz" */
#define WASM_OP_I64_POPCNT		0x7B /* "i64.popcnt" */
#define WASM_OP_I64_ADD			0x7C /* "i64.add" */
#define WASM_OP_I64_SUB			0x7D /* "i64.sub" */
#define WASM_OP_I64_MUL			0x7E /* "i64.mul" */
#define WASM_OP_I64_DIV_S		0x7F /* "i64.div_s" */

#define WASM_OP_I64_DIV_U		0x80 /* "i64.div_u" */
#define WASM_OP_I64_REM_S		0x81 /* "i64.rem_s" */
#define WASM_OP_I64_REM_U		0x82 /* "i64.rem_u" */
#define WASM_OP_I64_AND			0x83 /* "i64.and" */
#define WASM_OP_I64_OR			0x84 /* "i64.or" */
#define WASM_OP_I64_XOR			0x85 /* "i64.xor" */
#define WASM_OP_I64_SHL			0x86 /* "i64.shl" */
#define WASM_OP_I64_SHR_S		0x87 /* "i64.shr_s" */
#define WASM_OP_I64_SHR_U		0x88 /* "i64.shr_u" */
#define WASM_OP_I64_ROTL		0x89 /* "i64.rotl" */
#define WASM_OP_I64_ROTR		0x8A /* "i64.rotr" */
#define WASM_OP_F32_ABS     0x8B /* "f32.abs" */
#define WASM_OP_F32_NEG     0x8C /* "f32.neg" */
#define WASM_OP_F32_CEIL    0x8D /* "f32.ceil" */
#define WASM_OP_F32_FLOOR   0x8E /* "f32.floor" */
#define WASM_OP_F32_TRUNC   0x8F /* "f32.trunc" */

#define WASM_OP_F32_NEAREST 0x90 /* "f32.nearest" */
#define WASM_OP_F32_SQRT    0x91 /* "f32.sqrt" */
#define WASM_OP_F32_ADD			0x92 /* "f32.add" */
#define WASM_OP_F32_SUB			0x93 /* "f32.sub" */
#define WASM_OP_F32_MUL			0x94 /* "f32.mul" */
#define WASM_OP_F32_DIV			0x95 /* "f32.div" */
#define WASM_OP_F32_MIN			0x96 /* "f32.min" */
#define WASM_OP_F32_MAX			0x97 /* "f32.max" */
#define WASM_OP_F32_COPYSIGN			0x98 /* "f32.copysign" */
#define WASM_OP_F64_ABS     0x99 /* "f64.abs" */
#define WASM_OP_F64_NEG     0x9A /* "f64.neg" */
#define WASM_OP_F64_CEIL    0x9B /* "f64.ceil" */
#define WASM_OP_F64_FLOOR   0x9C /* "f64.floor" */
#define WASM_OP_F64_TRUNC   0x9D /* "f64.trunc" */
#define WASM_OP_F64_NEAREST 0x9E /* "f64.nearest" */
#define WASM_OP_F64_SQRT    0x9F /* "f64.sqrt" */

#define WASM_OP_F64_ADD			0xA0 /* "f64.add" */
#define WASM_OP_F64_SUB			0xA1 /* "f64.sub" */
#define WASM_OP_F64_MUL			0xA2 /* "f64.mul" */
#define WASM_OP_F64_DIV			0xA3 /* "f64.div" */
#define WASM_OP_F64_MIN			0xA4 /* "f64.min" */
#define WASM_OP_F64_MAX			0xA5 /* "f64.max" */
#define WASM_OP_F64_COPYSIGN			0xA6 /* "f64.copysign" */
#define WASM_OP_I32_WRAP_I64      0xA7 /* "i32.wrap_i64" */
#define WASM_OP_I32_TRUNC_F32_S		0xA8 /* "i32.trunc_f32_s" */
#define WASM_OP_I32_TRUNC_F32_U		0xA9 /* "i32.trunc_f32_u" */
#define WASM_OP_I32_TRUNC_F64_S		0xAA /* "i32.trunc_f64_s" */
#define WASM_OP_I32_TRUNC_F64_U		0xAB /* "i32.trunc_f64_u" */
#define WASM_OP_I64_EXTEND_I32_S		0xAC /* "i64.extend_i32_s" */
#define WASM_OP_I64_EXTEND_I32_U		0xAD /* "i64.extend_i32_u" */
#define WASM_OP_I64_TRUNC_F32_S		0xAE /* "i64.trunc_f32_s" */
#define WASM_OP_I64_TRUNC_F32_U		0xAF /* "i64.trunc_f32_u" */

#define WASM_OP_I64_TRUNC_F64_S		0xB0 /* "i64.trunc_f64_s" */
#define WASM_OP_I64_TRUNC_F64_U		0xB1 /* "i64.trunc_f64_u" */
#define WASM_OP_F32_CONVERT_I32_S	0xB2 /* "f32.convert_i32_s" */
#define WASM_OP_F32_CONVERT_I32_U	0xB3 /* "f32.convert_i32_u" */
#define WASM_OP_F32_CONVERT_I64_S	0xB4 /* "f32.convert_i64_s" */
#define WASM_OP_F32_CONVERT_I64_U	0xB5 /* "f32.convert_i64_u" */
#define WASM_OP_F32_DEMOTE_F64    0xB6 /* "f32.demote_f64" */
#define WASM_OP_F64_CONVERT_I32_S	0xB7 /* "f64.convert_i32_s" */
#define WASM_OP_F64_CONVERT_I32_U	0xB8 /* "f64.convert_i32_u" */
#define WASM_OP_F64_CONVERT_I64_S	0xB9 /* "f64.convert_i64_s" */
#define WASM_OP_F64_CONVERT_I64_U	0xBA /* "f64.convert_i64_u" */
#define WASM_OP_F64_PROMOTE_F32   0xBB /* "f64.promote_f32" */
#define WASM_OP_I32_REINTERPRET_F32 0xBC /* "i32.reinterpret_f32 */  
#define WASM_OP_I64_REINTERPRET_F64 0xBD /* "i64.reinterpret_f64 */  
#define WASM_OP_F32_REINTERPRET_I32 0xBE /* "f32.reinterpret_i32 */  
#define WASM_OP_F64_REINTERPRET_I64 0xBF /* "f64.reinterpret_i64 */  

#define WASM_OP_I32_EXTEND8_S		0xC0 /* "i32.extend8_s" */
#define WASM_OP_I32_EXTEND16_S		0xC1 /* "i32.extend16_s" */



// Constant array
static const char* opcode_names[] = {
};
