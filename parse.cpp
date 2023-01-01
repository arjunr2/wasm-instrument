#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include <stdexcept>

#include "ir.h"
#include "common.h"


#define FN_MAX_SIZE 1048576
#define BYTES_PER_LINE 32

/*** Parsing macros ***/
#define RD_U32()        read_u32leb(&buf)
#define RD_I32()        read_i32leb(&buf)
#define RD_BYTE()       read_u8(&buf)
#define RD_U32_RAW()    read_u32(&buf)
#define RD_U64()        read_u64(&buf)
#define RD_NAME(len)    read_name(&buf, &(len))
#define RD_BYTESTR(len) read_bytes(&buf, len)
/********************/


/**********************/
//static uint32_t construct_u32leb4(uint32_t val) {
//  uint32_t new_val;
//  *((byte*)(&new_val))        = 0x80 | (val & 0x7f);
//  *(((byte*)(&new_val)) + 1)  = 0x80 | ((val >> 7) & 0x7F);
//  *(((byte*)(&new_val)) + 2)  = 0x80 | ((val >> 14) & 0x7F);
//  *(((byte*)(&new_val)) + 3)  = 0x80 | ((val >> 21) & 0x7F);
//  return new_val;
//}
//
//// Returns string for given import/export description
//static const char* ie_desc_name(byte idxtype) {
//  switch (idxtype) {
//    case WASM_IE_DESC_FUNC: return "func";
//    case WASM_IE_DESC_TABLE: return "table";
//    case WASM_IE_DESC_MEM: return "memory";
//    case WASM_IE_DESC_GLOBAL: return "global";
//    default:
//      return "0xDEADBEEF";
//  }
//}
//
///* Function to read i32.const offset since 
//  data/elem section only uses these */
//static uint32_t decode_flag_and_i32_const_off_expr(string *sp, buffer_t *buf) {
//  APPEND_TAB(*sp);
//  uint32_t flag = read_u32leb(buf);
//  // Has to be 0 flag for this
//  if (flag != 0) {  ERR("Non-0 flag for data/elem section\n"); }
//  // Has to be i32.const offset for this
//  byte opc = read_u8(buf);
//  if (opc != WASM_OP_I32_CONST) {
//    ERR("Offset for data section has to be \"i32.const\"");
//  } 
//  // Offset val
//  uint32_t offset;
//  RD_APPEND_STR_U32(*sp, buf, offset);
//  
//  byte endopc = read_u8(buf);
//  if (endopc != WASM_OP_END) {
//    ERR("Offset for data section can't find end after i32.const\n");
//  }
//
//  FLUSH_STR(*sp);
//  return offset;
//}
//
//
///* Read LimitsType */
//wasm_limits_t read_limits(string *sp, buffer_t *buf) {
//  wasm_limits_t limits;
//  byte max_provided = read_u8(buf);
//  limits.has_max = max_provided;
//  /* Min */
//  RD_APPEND_STR_U32(*sp, buf, limits.initial);
//
//  if (max_provided) { RD_APPEND_STR_U32(*sp, buf, limits.max); }
//  return limits;
//}
//
///* Read TableType */
//void read_table_type(wasm_table_decl_t *table, string *sp, buffer_t *buf) {
//    byte reftype = read_u8(buf);
//    APPEND_STR(*sp, type_name(reftype));
//    APPEND_SPACE(*sp);
//    table->limits = read_limits(sp, buf);
//}
//
///* Read GlobalType */
//void read_global_type(wasm_global_decl_t* global, string *sp, buffer_t *buf) {
//  byte type = read_u8(buf);
//  APPEND_STR(*sp, type_name(type));
//  APPEND_SPACE(*sp);
//
//  byte mutable = read_u8(buf);
//  if (mutable) { APPEND_STR(*sp, "mutable "); }
//
//  global->type = type;
//  global->mutable = mutable;
//}


//static wasm_type_t* read_type_list(uint32_t num, buffer_t *buf) {
//  MALLOC(types, wasm_type_t, num);
//  /* Add all types for params */
//  for (uint32_t j = 0; j < num; j++) {
//    types[j] = (wasm_type_t)(RD_BYTE());
//  }
//  return types;
//}

void WasmModule::decode_type_section(buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_sigs = read_u32leb(buf);
//
//  MALLOC(sigs, wasm_sig_decl_t, num_sigs);
//
//  for (uint32_t i = 0; i < num_sigs; i++) {
//    wasm_sig_decl_t* sig = sigs + i;
//    /* Read type */
//    byte type = RD_BYTE();
//    if (type != WASM_TYPE_FUNC) {
//      ERR("Has to be func type for signature!\n");
//      return;
//    }
//
//    /* For params */
//    sig->num_params = RD_U32();
//    sig->params = read_type_list(sig->num_params, buf);
//    /* For results */
//    sig->num_results = read_u32leb(buf);
//    sig->results = read_type_list(sig->num_results, buf);
//  }
//
//  module->num_sigs = num_sigs;
//  module->sigs = sigs;
}


void WasmModule::decode_import_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_imports = read_u32leb(buf);
//  PRINT_SEC_HEADER(import, num_imports);
//
//  MALLOC(imports, wasm_import_decl_t, num_imports);
//
//  ALLOC_STR(s);
//  for (uint32_t i = 0; i < num_imports; i++) {
//    wasm_import_decl_t *import = &imports[i];
//
//    APPEND_TAB(s);
//    /* Module */
//    RD_APPEND_STR_STRING(s, buf, import->mod_name_length, import->mod_name);
//    APPEND_SPACE(s);
//    
//    /* Import name */
//    RD_APPEND_STR_STRING(s, buf, import->member_name_length, import->member_name);
//    APPEND_SPACE(s);
//
//    /* Import type and args */    
//    byte idxtype = read_u8(buf);
//    APPEND_STR(s, ie_desc_name(idxtype));
//    APPEND_SPACE(s);
//
//    import->kind = idxtype;
//    switch(idxtype) {
//      case WASM_IE_DESC_FUNC:   RD_APPEND_STR_U32(s, buf, import->index); break;
//      default:
//        ERR("Invalid Index type found!\n");
//    }
//    FLUSH_STR(s);
//
//  }
//  DELETE_STR(s);
//
//  module->num_imports = num_imports;
//  module->imports = imports;
}

void WasmModule::decode_function_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_funcs = read_u32leb(buf);
//  
//  uint32_t num_imports = module->num_imports;
//  MALLOC(funcs, wasm_func_decl_t, num_imports + num_funcs);
//
//  ALLOC_STR(s);
//  for (uint32_t i = 0; i < num_imports; i++) {
//    wasm_func_decl_t *func = &funcs[i];
//    func->sig_index = module->imports[i].index;
//    func->sig = &module->sigs[func->sig_index];
//  }
//  for (uint32_t i = 0; i < num_funcs; i++) {
//    wasm_func_decl_t *func = &funcs[i + num_imports];
//    APPEND_TAB(s);
//
//    /* Get signature idx */
//    uint32_t idx = read_u32leb(buf);
//    APPEND_STR_U32(s, idx);
//    func->sig_index = idx;
//    func->sig = &module->sigs[idx];
//    
//    /* Print fn idx */
//    FLUSH_STR(s);
//  }
//  DELETE_STR(s);
//
//  module->num_funcs = num_funcs;
//  module->funcs = funcs;
}


void WasmModule::decode_table_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_tabs = read_u32leb(buf);
//
//  PRINT_SEC_HEADER(table, num_tabs);
//
//  MALLOC(tables, wasm_table_decl_t, num_tabs);
//
//  ALLOC_STR(s);
//
//  for (uint32_t i = 0; i < num_tabs; i++) {
//    wasm_table_decl_t *table = &tables[i];
//    APPEND_TAB(s);
//    read_table_type(table, &s, buf);
//    FLUSH_STR(s);
//  }
//
//  DELETE_STR(s);
//  module->num_tables = num_tabs;
//  module->table = tables;
}


void WasmModule::decode_memory_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_mems = read_u32leb(buf);
//  if (num_mems != 1) {
//    ERR("Memory component has to be 1!\n");
//  }
//
//  PRINT_SEC_HEADER(memory, num_mems);
//
//  ALLOC_STR(s);
//
//  APPEND_TAB(s);
//  module->num_mems = num_mems;
//  module->mem_limits = read_limits(&s, buf);
//  FLUSH_STR(s);
//
//  DELETE_STR(s);
}


void WasmModule::decode_export_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_exps = read_u32leb(buf);
//  PRINT_SEC_HEADER(export, num_exps);
//  
//  MALLOC(exports, wasm_export_decl_t, num_exps);
//
//  ALLOC_STR(s);
//  /* String + exp descriptor + idx */
//  for (uint32_t i = 0; i < num_exps; i++) {
//    wasm_export_decl_t *export = &exports[i];
//    APPEND_TAB(s);
//
//    /* Export descriptor */
//    RD_APPEND_STR_STRING(s, buf, export->length, export->name);
//    APPEND_SPACE(s);
//    
//    byte idxtype = read_u8(buf);
//    APPEND_STR(s, ie_desc_name(idxtype));
//    APPEND_SPACE(s);
//    export->kind = idxtype;
//     
//    RD_APPEND_STR_U32(s, buf, export->index);
//    FLUSH_STR(s);
//  }
//  DELETE_STR(s);
//
//  module->num_exports = num_exps;
//  module->exports = exports;
}


void WasmModule::decode_start_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t fn_idx = read_u32leb(buf);
//  PRINT_SEC_HEADER(start, fn_idx);
//
//  module->has_start = true;
//  module->start_idx = fn_idx;
}


void WasmModule::decode_element_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_elem = read_u32leb(buf);
//  PRINT_SEC_HEADER(element, num_elem);
//
//  MALLOC(elems, wasm_elems_decl_t, num_elem);
//
//  ALLOC_STR(s);
//  for (uint32_t i = 0; i < num_elem; i++) {
//    wasm_elems_decl_t *elem = &elems[i];
//    /* Offset val */
//    elem->table_offset = decode_flag_and_i32_const_off_expr(&s, buf);
//
//    /* Element fn idx vector */
//    APPEND_TAB(s);
//    uint32_t num_idxs = read_u32leb(buf);
//    APPEND_STR_U32(s, num_idxs);
//    FLUSH_STR(s);
//
//    elem->length = num_idxs;
//
//    MALLOC(func_idxs, uint32_t, num_idxs);
//
//    APPEND_TAB(s);
//    for (uint32_t j = 0; j < num_idxs; j++) {
//      RD_APPEND_STR_U32(s, buf, func_idxs[j]);
//    }
//    elem->func_indexes = func_idxs;
//
//    APPEND_STR(s, "\n");
//    FLUSH_STR(s);
//  }
//  DELETE_STR(s);
//
//  module->num_elems = num_elem;
//  module->elems = elems;
}


//static void decode_locals(wasm_func_decl_t *fn, buffer_t *buf) {
//
//  ALLOC_STR(s);
//  /* Write num local elements */
//  uint32_t num_elems = read_u32leb(buf);
//  APPEND_TAB(s);
//  APPEND_STR_U32(s, num_elems);
//  FLUSH_STR(s);
//
//  MALLOC(locals, wasm_local_decl_t, num_elems);
//  
//  uint32_t num_locals = 0;
//  /* Write local types */
//  if (num_elems != 0) {
//    APPEND_TAB(s);
//    for (uint32_t i = 0; i < num_elems; i++) {
//      /* Number of locals of type */
//      uint32_t num = read_u32leb(buf);
//      APPEND_STR_U32(s, num);
//      num_locals += num;
//      /* Type */
//      byte type = read_u8(buf);
//      APPEND_STR(s, type_name(type));
//      APPEND_SPACE(s);
//
//      locals[i].count = num;
//      locals[i].type = type;
//    }
//    FLUSH_STR(s);
//  }
//  
//  fn->num_local_vec = num_elems;
//  fn->num_locals = num_locals;
//  fn->local_decl = locals;
//
//  DELETE_STR(s);
//}
//
//block_list_t static_blocks[FN_MAX_SIZE];
//
//void decode_expr(buffer_t *buf, bool flush, uint32_t base_tab, bool replace_last) {
//  ALLOC_STR(s);
//
//  uint32_t _;
//
//  /* For br replacement */
//  byte* target_addr;
//  uint32_t idx;
//  uint32_t static_idx;
//  uint32_t offset;
//  int32_t print_val;
//  /* */
//
//  int static_ctr = 0;
//  int num_brs = 0;
//  int dyn2static_idxs[FN_MAX_SIZE];
//
//  uint32_t scope = 0;
//  // Initial scope
//  byte opcode = WASM_OP_BLOCK;
//  STATIC_BR_START_BLOCK()
//  scope++;
//
//  bool end_of_expr = false;
//  while (1) {
//    /* Pretty indent */
//    for (int i = 0; i <= scope + base_tab; i++) { APPEND_TAB(s); }
//
//    /* Decode insn */
//    byte opcode = read_u8(buf);
//    const char* opname = opcode_names[opcode];
//    switch (opcode) {
//      
//      case WASM_OP_UNREACHABLE:		/* "unreachable" */
//      case WASM_OP_NOP:			/* "nop" */
//      
//      case WASM_OP_RETURN:			/* "return" */
//      
//      case WASM_OP_DROP:			/* "drop" */
//      case WASM_OP_SELECT:			/* "select" */
//
//      case WASM_OP_I32_EQZ:			/* "i32.eqz" */
//      case WASM_OP_I32_EQ:			/* "i32.eq" */
//      case WASM_OP_I32_NE:			/* "i32.ne" */
//      case WASM_OP_I32_LT_S:		/* "i32.lt_s" */
//      case WASM_OP_I32_LT_U:		/* "i32.lt_u" */
//      case WASM_OP_I32_GT_S:		/* "i32.gt_s" */
//      case WASM_OP_I32_GT_U:		/* "i32.gt_u" */
//      case WASM_OP_I32_LE_S:		/* "i32.le_s" */
//      case WASM_OP_I32_LE_U:		/* "i32.le_u" */
//      case WASM_OP_I32_GE_S:		/* "i32.ge_s" */
//      case WASM_OP_I32_GE_U:		/* "i32.ge_u" */
//
//      case WASM_OP_F64_EQ:			/* "f64.eq" */
//      case WASM_OP_F64_NE:			/* "f64.ne" */
//      case WASM_OP_F64_LT:			/* "f64.lt" */
//      case WASM_OP_F64_GT:			/* "f64.gt" */
//      case WASM_OP_F64_LE:			/* "f64.le" */
//      case WASM_OP_F64_GE:			/* "f64.ge" */
//
//      case WASM_OP_I32_CLZ:			/* "i32.clz" */
//      case WASM_OP_I32_CTZ:			/* "i32.ctz" */
//      case WASM_OP_I32_POPCNT:		/* "i32.popcnt" */
//      
//      case WASM_OP_I32_ADD:			/* "i32.add" */
//      case WASM_OP_I32_SUB:			/* "i32.sub" */
//      case WASM_OP_I32_MUL:			/* "i32.mul" */
//      case WASM_OP_I32_DIV_S:		/* "i32.div_s" */
//      case WASM_OP_I32_DIV_U:		/* "i32.div_u" */
//      case WASM_OP_I32_REM_S:		/* "i32.rem_s" */
//      case WASM_OP_I32_REM_U:		/* "i32.rem_u" */
//      case WASM_OP_I32_AND:			/* "i32.and" */
//      case WASM_OP_I32_OR:			/* "i32.or" */
//      case WASM_OP_I32_XOR:			/* "i32.xor" */
//      case WASM_OP_I32_SHL:			/* "i32.shl" */
//      case WASM_OP_I32_SHR_S:		/* "i32.shr_s" */
//      case WASM_OP_I32_SHR_U:		/* "i32.shr_u" */
//      case WASM_OP_I32_ROTL:		/* "i32.rotl" */
//      case WASM_OP_I32_ROTR:		/* "i32.rotr" */
//      
//      case WASM_OP_F64_ADD:			/* "f64.add" */
//      case WASM_OP_F64_SUB:			/* "f64.sub" */
//      case WASM_OP_F64_MUL:			/* "f64.mul" */
//      case WASM_OP_F64_DIV:			/* "f64.div" */
//      
//      case WASM_OP_I32_TRUNC_F64_S:		/* "i32.trunc_f64_s" */
//      case WASM_OP_I32_TRUNC_F64_U:		/* "i32.trunc_f64_u" */
//      
//      case WASM_OP_F64_CONVERT_I32_S:	/* "f64.convert_i32_s" */
//      case WASM_OP_F64_CONVERT_I32_U:	/* "f64.convert_i32_u" */
//      case WASM_OP_F64_CONVERT_I64_S:	/* "f64.convert_i64_s" */
//      case WASM_OP_F64_CONVERT_I64_U:	/* "f64.convert_i64_u" */
//      
//      case WASM_OP_I32_EXTEND8_S:		/* "i32.extend8_s" */
//      case WASM_OP_I32_EXTEND16_S:		/* "i32.extend16_s" */
//          NO_ARG_INSN();
//
//      case WASM_OP_BLOCK:		/* "block" BLOCKT */
//      case WASM_OP_LOOP:			/* "loop" BLOCKT */
//      case WASM_OP_IF:			/* "if" BLOCKT */
//          SCOPE_BEGIN_INSN();
//
//      case WASM_OP_ELSE:			/* "else" */
//          STRIP_STR(s, 2);
//          OPCODE();
//          break;
//
//      case WASM_OP_END:			/* "end" */
//          STRIP_STR(s, 2);
//          OPCODE();
//          SCOPE_COM_DEC(s);
//          end_of_expr = (scope == 0);
//          break;
//
//
//      case WASM_OP_BR:			/* "br" LABEL */
//      case WASM_OP_BR_IF: 			/* "br_if" LABEL */
//        OPCODE();
//        BR_REPLACE_OFFSET();
//        num_brs++;
//        break;
//
//      case WASM_OP_CALL:			/* "call" FUNC */
//      
//      case WASM_OP_LOCAL_GET:		/* "local.get" LOCAL */
//      case WASM_OP_LOCAL_SET:		/* "local.set" LOCAL */
//      case WASM_OP_LOCAL_TEE:		/* "local.tee" LOCAL */
//      case WASM_OP_GLOBAL_GET:		/* "global.get" GLOBAL */
//      case WASM_OP_GLOBAL_SET:		/* "global.set" GLOBAL */
//      case WASM_OP_TABLE_GET:		/* "table.get" TABLE */
//      case WASM_OP_TABLE_SET:		/* "table.set" TABLE */
//          ONE_U32_INSN();
//
//      case WASM_OP_BR_TABLE: 		/* "br_table" LABELS */
//          OPCODE();
//          /* Label vector */
//          uint32_t num_elems = read_u32leb(buf);
//          APPEND_STR_U32(s, num_elems);
//          for (uint32_t i = 0; i < num_elems; i++) {
//            //RD_APPEND_STR_U32(s, buf, _);
//            BR_REPLACE_OFFSET();
//          }
//          /* Label index */
//          //RD_APPEND_STR_U32(s, buf, _);
//          BR_REPLACE_OFFSET();
//          break;
//      
//      
//      
//      case WASM_OP_CALL_INDIRECT:		/* "call_indirect" SIG_TABLE */
//          OPCODE();
//          /* Type idx */
//          RD_APPEND_STR_U32(s, buf, _);
//          /* Table idx */
//          RD_APPEND_STR_U32(s, buf, _);
//          break;
//      
//      case WASM_OP_I32_LOAD:		/* "i32.load" MEMARG */
//      case WASM_OP_F64_LOAD:		/* "f64.load" MEMARG */
//      case WASM_OP_I32_LOAD8_S:		/* "i32.load8_s" MEMARG */
//      case WASM_OP_I32_LOAD8_U:		/* "i32.load8_u" MEMARG */
//      case WASM_OP_I32_LOAD16_S:		/* "i32.load16_s" MEMARG */
//      case WASM_OP_I32_LOAD16_U:		/* "i32.load16_u" MEMARG */
//      case WASM_OP_I32_STORE:		/* "i32.store" MEMARG */
//      case WASM_OP_F64_STORE:		/* "f64.store" MEMARG */
//      case WASM_OP_I32_STORE8:		/* "i32.store8" MEMARG */
//      case WASM_OP_I32_STORE16:	/* "i32.store16" MEMARG */
//          MEMARG_INSN();
//      
//      case WASM_OP_I32_CONST:		/* "i32.const" I32 */
//          ONE_I32_INSN();
//      
//      case WASM_OP_F64_CONST:		/* "f64.const" F64 */
//          OPCODE();
//          uint64_t val = read_u64(buf);
//          APPEND_STR_HEX64(s, val);
//          break;
//      
//      
//      default:
//          ERR("UNKNOWN OPCODE -- %u\n", opcode);
//    }
//    if (flush) { FLUSH_STR(s); }
//    if (end_of_expr) { break; }
//  }
//  
//  if (flush) { FLUSH_STR(s); }
//
//  DELETE_STR(s);
//}


/* Gets run after function section; in order */
void WasmModule::decode_code_section (buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_fn = read_u32leb(buf);
//  PRINT_SEC_HEADER(code, num_fn);
//
//  ALLOC_STR(s);
//  for (uint32_t i = 0; i < num_fn; i++) {
//    uint32_t idx = module->num_imports + i;
//    /* Fn size */
//    uint32_t size = read_u32leb(buf);
//    APPEND_TAB(s);
//    APPEND_STR(s, "body ");
//    APPEND_STR_U32(s, size);
//    FLUSH_STR(s);
//
//    /* Parse body */
//    /* Local section */
//    decode_locals(&module->funcs[idx], buf);
//
//    module->funcs[idx].code_start = buf.ptr;
//    /* Fn body: Instruction decoding */
//    decode_expr(buf, false, 1, false);
//    module->funcs[idx].code_end = buf.ptr;
//    if (replace_brs) {
//      /* Branch replacement */
//      buf.ptr = module->funcs[idx].code_start;
//      decode_expr(buf, true, 1, true);
//    }
//  }
//  DELETE_STR(s);
}


void WasmModule::decode_global_section(buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_globs = read_u32leb(buf);
//  PRINT_SEC_HEADER(global, num_globs);
//
//  MALLOC(globals, wasm_global_decl_t, num_globs);
//
//  ALLOC_STR(s);
//  for (uint32_t i = 0; i < num_globs; i++) {
//    wasm_global_decl_t *global = &globals[i];
//    APPEND_TAB(s);
//    read_global_type(global, &s, buf);
//    FLUSH_STR(s);
//
//    global->init_expr_start = buf.ptr;
//    decode_expr(buf, true, 1, false);
//    global->init_expr_end = buf.ptr;
//  }
//  DELETE_STR(s);
//
//  module->num_globals = num_globs;
//  module->globals = globals;
}


void WasmModule::decode_data_section(buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  uint32_t num_datas = RD_U32();
//
//  MALLOC(datas, wasm_data_decl_t, num_datas);
//
//  for (uint32_t i = 0; i < num_data; i++) {
//    wasm_data_decl_t *data = datas + i;
//    /* Offset val */
//    data->mem_offset = decode_flag_and_i32_const_off_expr(&s, buf);
//    /* Size val */
//    APPEND_TAB(s);
//    uint32_t sz = read_u32leb(buf);
//    APPEND_STR_U32(s, sz);
//    FLUSH_STR(s);
//
//    data->bytes_start = buf.ptr;
//    data->bytes_end = buf.ptr + sz;
//    /* Print bytes */
//    for (uint32_t i = 0; i < sz; i++) {
//      if ((i & (BYTES_PER_LINE-1)) == 0) { 
//        if (i != 0) { FLUSH_STR(s); }
//        APPEND_TAB(s);
//      }
//      byte b = read_u8(buf);
//      APPEND_STR_BYTE(s, b);
//    }
//    APPEND_STR(s, "\n");
//    FLUSH_STR(s);
//  }
//  DELETE_STR(s);
//
//  module->num_data = num_datas;
//  module->data = datas;
}

void WasmModule::decode_datacount_section(buffer_t &buf, uint32_t len) {
  this->has_datacount = true;
  this->num_datas = RD_U32();
}

void WasmModule::decode_custom_section(buffer_t &buf, uint32_t len) {
  buf.ptr += len;
//  const byte* end_sec = buf.ptr + len;
//  /* Cap at 8 custom sections */
//  int i = module->num_customs++;
//  if (i == 0) {
//    MALLOC(customs, wasm_custom_decl_t, 8);
//    module->customs = customs;
//  } else if (i >= 8) {
//    ERR ("More than 8 custom sections encountered! Cannot support\n");
//    return;
//  }
//
//  wasm_custom_decl_t* custom = module->customs + i;
//
//  custom->name = RD_NAME(custom->name_length);
//  custom->bytes_length = end_sec - buf.ptr;
//  custom->bytes = RD_BYTESTR(custom->bytes_length);
}




void decode_sections(WasmModule &module, buffer_t &buf) {

  while (buf.ptr < buf.end) {
    byte section_id = RD_BYTE();
    uint32_t len = RD_U32();

    TRACE("Found section \"%s\", len: %d\n", wasm_section_name(section_id), len);

    buffer_t cbuf = {buf.ptr, buf.ptr, buf.ptr + len};
    #define DECODE_CALL(sec)  module.decode_##sec##_section (cbuf, len); break;
    switch (section_id) {
      case WASM_SECT_TYPE:     DECODE_CALL(type); 
      case WASM_SECT_IMPORT:   DECODE_CALL(import); 
      case WASM_SECT_FUNCTION: DECODE_CALL(function); 
      case WASM_SECT_TABLE:    DECODE_CALL(table); 
      case WASM_SECT_MEMORY:   DECODE_CALL(memory); 
      case WASM_SECT_GLOBAL:   DECODE_CALL(global); 
      case WASM_SECT_EXPORT:   DECODE_CALL(export); 
      case WASM_SECT_START:    DECODE_CALL(start); 
      case WASM_SECT_ELEMENT:  DECODE_CALL(element);  
      case WASM_SECT_CODE:     DECODE_CALL(code); 
      case WASM_SECT_DATA:     DECODE_CALL(data); 
      case WASM_SECT_DATACOUNT:DECODE_CALL(datacount); 
      case WASM_SECT_CUSTOM:   DECODE_CALL(custom); 
      default:
        ERR("Unknown section id: %u\n", section_id);
        return;
    }

    if (cbuf.ptr != cbuf.end) {
      ERR("Section \"%s\" not aligned after parsing -- start:%lu, ptr:%lu, end:%lu\n", 
          wasm_section_name(section_id),
          cbuf.start - buf.start, 
          cbuf.ptr - buf.start, 
          cbuf.end - buf.start);
      return;
    }
    // Advance section
    buf.ptr = cbuf.ptr;
  }
}


/* Main Parse routine */
WasmModule parse_bytecode(const byte* start, const byte* end) {
  
  WasmModule module = {};

  /* Initialize buffer */
  buffer_t main_buf = {start, start, end};
  buffer_t &buf = main_buf;

  if (buf.ptr == buf.end) { 
    throw runtime_error("Empty bytecode");
  }

  /* Magic number & Version */
  uint32_t magic = RD_U32_RAW();
  if (magic != WASM_MAGIC) {
    throw runtime_error("Parse | Wasm Magic Value");
  }

  uint32_t version = RD_U32_RAW();
  if (version != WASM_VERSION) {
    throw runtime_error("Parse | Wasm Version");
  }

  module.magic = magic;
  module.version = version;

  decode_sections(module, buf);
  
  /* Has to match exactly */
  if (buf.ptr != buf.end) {
    throw runtime_error("Parse | Unexpected end");
  }
  return module;
}
