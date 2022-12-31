#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "ir.h"
#include "common.h"


#define FN_MAX_SIZE 1048576
#define BYTES_PER_LINE 32

#define PRINT_SEC_HEADER(type, length)  \
  DISASS(#type " %u\n", length);

/*** STRING MACROS ***/
#define APPEND_STR(s, str) \
  if (g_disassemble) { s = strappend(s, str); }

#define APPEND_TAB(s) \
  if (g_disassemble) { s = strappend(s, "  "); }

#define APPEND_SPACE(s) \
  if (g_disassemble) { s = strappend(s, " "); }

#define APPEND_STR_U32(s, val) \
  if (g_disassemble) { s = strappend_int32(s, val, false); }

#define APPEND_STR_I32(s, val) \
  if (g_disassemble) { s = strappend_int32(s, val, true); }

#define APPEND_STR_BYTE(s, val) \
  if (g_disassemble) { s = strappend_byte(s, val); }

#define APPEND_STR_HEX64(s, val) \
  if (g_disassemble) { s = strappend_hex64(s, val); }

#define ALLOC_STR(s) \
  string s = stralloc(); \
  strclear(s);

#define DELETE_STR(s) \
  strdelete(s);

#define STRIP_STR(s, n) \
  if (g_disassemble) { s = strip_chars(s, n); }


#define RD_APPEND_STR_U32(s, buf, res) ({ \
  uint32_t val = read_u32leb(buf); \
  APPEND_STR_U32(s, val);  \
  res = val; \
})

#define RD_APPEND_STR_I32(s, buf) ({ \
  uint32_t val = read_i32leb(buf); \
  APPEND_STR_I32(s, val);  \
})


#define RD_APPEND_STR_STRING(s, buf, len, res) ({ \
  char *str_val = read_string(buf, &len); \
  APPEND_STR_U32(s, len); \
  APPEND_STR(s, "\"");  \
  APPEND_STR(s, str_val); \
  APPEND_STR(s, "\"");  \
  APPEND_SPACE(s); \
  res = str_val;  \
})
/********************/

/*** FLUSH OPS ***/
#define FLUSH_STR(s) \
  DISASS("%s\n", (s).v);  \
  strclear(s);

#define FLUSH_BYTE(b) \
  DISASS("%02X ", b); \

/********************/

#define STATIC_BR_START_BLOCK() \
  static_blocks[static_ctr].op = opcode; \
  static_blocks[static_ctr].start_addr = (byte*) buf->ptr; \
  dyn2static_idxs[scope] = static_ctr;  \
  static_ctr++;

#define STATIC_BR_END_BLOCK() \
  static_blocks[ dyn2static_idxs[scope] ].end_addr = (byte*)buf->ptr - 1;

/*** INSN MACROS ***/
#define OPCODE()  \
  APPEND_STR(s, opname); \
  APPEND_SPACE(s);

#define SCOPE_COM(s) \
  APPEND_STR(s, " ;; s"); \
  APPEND_STR_U32(s, scope); \
  scope++;  \

#define NO_ARG_INSN() \
  APPEND_STR(s, opname); break;

#define ONE_U32_INSN() \
  OPCODE(); \
  RD_APPEND_STR_U32(s, buf, _);  \
  break;

#define ONE_I32_INSN() \
  OPCODE(); \
  RD_APPEND_STR_I32(s, buf);  \
  break;

#define SCOPE_BEGIN_INSN() \
  APPEND_STR(s, opname);  \
  /* Skip the blocktype */  \
  uint32_t blocktype = read_u32leb(buf);  \
  STATIC_BR_START_BLOCK();  \
  SCOPE_COM(s); \
  break;

#define MEMARG_INSN() \
  OPCODE(); \
  uint32_t v = read_u32leb(buf);  \
  RD_APPEND_STR_U32(s, buf, _);  \
  break;

#define SCOPE_COM_DEC(s) \
  scope--;  \
  STATIC_BR_END_BLOCK();  \
  APPEND_STR(s, " ;; s"); \
  APPEND_STR_U32(s, scope);

/***********************/

#define BR_REPLACE_OFFSET() \
    idx = read_u32leb(buf);  \
    /* Get static idx of block*/  \
    static_idx = dyn2static_idxs[scope-idx-1]; \
    if (replace_last) { \
      block_list_t block_dets = static_blocks[ static_idx ];  \
      target_addr = ((block_dets.op == WASM_OP_LOOP) ?  \
                            block_dets.start_addr : \
                            block_dets.end_addr); \
      offset = target_addr - buf->ptr;  \
      memcpy((void*)(buf->ptr - 4), &offset, 4); \
    } \
    print_val = replace_last ? offset : idx;  \
    APPEND_STR_I32(s, print_val);

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
//// Returns string for given type
//static const char* type_name(byte type) {
//  switch (type) {
//    case WASM_TYPE_I32: return "i32";
//    case WASM_TYPE_F64: return "f64";
//    case WASM_TYPE_EXTERNREF: return "externref";
//    case WASM_TYPE_FUNCREF: return "funcref";
//    default:
//      return "0xDEADBEEF";
//  }
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
//
//
//static wasm_type_t* read_type_list(uint32_t num, string *sp, buffer_t *buf) {
//  MALLOC(types, wasm_type_t, num);
//
//  APPEND_STR_U32(*sp, num);
//  /* Add all types for params */
//  for (uint32_t j = 0; j < num; j++) {
//    byte type = read_u8(buf);
//    APPEND_STR(*sp, type_name(type));
//    APPEND_SPACE(*sp);
//
//    types[j] = type;
//  }
//  return types;
//}

void decode_type_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
//  uint32_t num_types = read_u32leb(buf);
//  PRINT_SEC_HEADER(type, num_types);
//
//  MALLOC(sigs, wasm_sig_decl_t, num_types);
//
//  ALLOC_STR(s);
//  for (uint32_t i = 0; i < num_types; i++) {
//    wasm_sig_decl_t* sig = &sigs[i];
//    /* Read type */
//    byte kind = read_u8(buf);
//    if (kind != WASM_KIND_FUNC) {
//      ERR("Has to be func type for signature!\n");
//      return;
//    }
//
//    APPEND_TAB(s);
//    APPEND_STR(s, "sig ");
//    
//    /* For params */
//    sig->num_params = read_u32leb(buf);
//    sig->params = read_type_list(sig->num_params, &s, buf);
//    /* For results */
//    sig->num_results = read_u32leb(buf);
//    sig->results = read_type_list(sig->num_results, &s, buf);
//
//    FLUSH_STR(s);
//  }
//  DELETE_STR(s);
//
//  module->num_sigs = num_types;
//  module->sigs = sigs;
}


void decode_import_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
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

void decode_function_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
//  uint32_t num_funcs = read_u32leb(buf);
//  PRINT_SEC_HEADER(function, num_funcs);
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


void decode_table_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
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


void decode_memory_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
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


void decode_export_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
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


void decode_start_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
//  uint32_t fn_idx = read_u32leb(buf);
//  PRINT_SEC_HEADER(start, fn_idx);
//
//  module->has_start = true;
//  module->start_idx = fn_idx;
}


void decode_element_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
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
void decode_code_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
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
//    module->funcs[idx].code_start = buf->ptr;
//    /* Fn body: Instruction decoding */
//    decode_expr(buf, false, 1, false);
//    module->funcs[idx].code_end = buf->ptr;
//    if (replace_brs) {
//      /* Branch replacement */
//      buf->ptr = module->funcs[idx].code_start;
//      decode_expr(buf, true, 1, true);
//    }
//  }
//  DELETE_STR(s);
}


void decode_global_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
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
//    global->init_expr_start = buf->ptr;
//    decode_expr(buf, true, 1, false);
//    global->init_expr_end = buf->ptr;
//  }
//  DELETE_STR(s);
//
//  module->num_globals = num_globs;
//  module->globals = globals;
}


void decode_data_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
//  uint32_t num_data = read_u32leb(buf);
//  PRINT_SEC_HEADER(data, num_data);
//
//  MALLOC(datas, wasm_data_decl_t, num_data);
//
//  ALLOC_STR(s);
//  for (uint32_t i = 0; i < num_data; i++) {
//    wasm_data_decl_t *data = &datas[i];
//    /* Offset val */
//    data->mem_offset = decode_flag_and_i32_const_off_expr(&s, buf);
//    /* Size val */
//    APPEND_TAB(s);
//    uint32_t sz = read_u32leb(buf);
//    APPEND_STR_U32(s, sz);
//    FLUSH_STR(s);
//
//    data->bytes_start = buf->ptr;
//    data->bytes_end = buf->ptr + sz;
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
//  module->num_data = num_data;
//  module->data = datas;
}

void decode_datacount_section(wasm_module_t* module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
}

void decode_custom_section(wasm_module_t *module, buffer_t *buf, uint32_t len) {
  buf->ptr += len;
//  PRINT_SEC_HEADER(custom, len);
//  const byte* start_sec = buf->ptr;
//  
//  ALLOC_STR(s);
//
//  APPEND_TAB(s);
//  uint32_t _;
//  char * __;
//  RD_APPEND_STR_STRING(s, buf, _, __);
//  FLUSH_STR(s);
//  
//  DELETE_STR(s);
//  
//  buf->ptr = start_sec + len;
}




// Returns the string name of a section code.
static const char* section_name(byte code) {
  switch (code) {
  case WASM_SECT_TYPE: return "type";
  case WASM_SECT_IMPORT: return "import";
  case WASM_SECT_FUNCTION: return "function";
  case WASM_SECT_TABLE: return "table";
  case WASM_SECT_MEMORY: return "memory";
  case WASM_SECT_GLOBAL: return "global";
  case WASM_SECT_EXPORT: return "export";
  case WASM_SECT_START: return "start";
  case WASM_SECT_ELEMENT: return "element";
  case WASM_SECT_CODE: return "code";
  case WASM_SECT_DATA: return "data";
  case WASM_SECT_DATACOUNT: return "datacount";
  case WASM_SECT_CUSTOM: return "custom";
  default:
    return "unknown";
  }
}


void decode_sections(wasm_module_t *module, buffer_t *buf) {

  while (buf->ptr < buf->end) {
    byte section_id = read_u8(buf);
    uint32_t len = read_u32leb(buf);

    TRACE("Found section \"%s\", len: %d\n", section_name(section_id), len);

    buffer_t cbuf = {buf->ptr, buf->ptr, buf->ptr + len};

    switch (section_id) {
      case WASM_SECT_TYPE:      decode_type_section(module, &cbuf, len); break;
      case WASM_SECT_IMPORT:    decode_import_section(module, &cbuf, len); break;
      case WASM_SECT_FUNCTION:  decode_function_section(module, &cbuf, len); break;
      case WASM_SECT_TABLE:     decode_table_section(module, &cbuf, len); break;
      case WASM_SECT_MEMORY:    decode_memory_section(module, &cbuf, len); break;
      case WASM_SECT_GLOBAL:    decode_global_section(module, &cbuf, len); break;
      case WASM_SECT_EXPORT:    decode_export_section(module, &cbuf, len); break;
      case WASM_SECT_START:     decode_start_section(module, &cbuf, len); break;
      case WASM_SECT_ELEMENT:   decode_element_section(module, &cbuf, len); break;
      case WASM_SECT_CODE:      decode_code_section(module, &cbuf, len); break;
      case WASM_SECT_DATA:      decode_data_section(module, &cbuf, len); break;
      case WASM_SECT_DATACOUNT: decode_datacount_section(module, &cbuf, len); break;
      case WASM_SECT_CUSTOM:    decode_custom_section(module, &cbuf, len); break;
      default:
        ERR("Unknown section id: %u\n", section_id);
    }

    if (cbuf.ptr != cbuf.end) {
      ERR("Section \"%s\" not aligned after parsing -- start:%lu, ptr:%lu, end:%lu\n", 
          section_name(section_id),
          cbuf.start - buf->start, 
          cbuf.ptr - buf->start, 
          cbuf.end - buf->start);
      return;
    }

    // Advance section
    buf->ptr = cbuf.ptr;
  }
}

//========= BEGIN SOLUTION ==========

wasm_module_t parse_bytecode(const byte* start, const byte* end) {
  
  wasm_module_t module = { 0 };
  
  /* Initialize buffer */
  buffer_t buf = {start, start, end};

  if (buf.ptr == buf.end) { 
    ERR("Empty bytecode\n");
    return module; 
  }

  uint32_t _;
  
  /* Magic number & Version */
  uint32_t magic = read_u32(&buf);
  if (magic != WASM_MAGIC) {
    ERR("Error: Wasm Magic Value (%u)\n", magic);
    return module;
  }

  uint32_t version = read_u32(&buf);
  if (version != WASM_VERSION) {
    ERR("Error: Wasm Version (%u)\n", version);
    return module;
  }

  decode_sections(&module, &buf);
  
  /* Has to match exactly */
  if (buf.ptr != buf.end) {
    ERR("Parse Error: Unexpected end\n");
  }
  return module;
}
