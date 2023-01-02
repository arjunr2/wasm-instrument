#include <stdexcept>
#include <iostream>

#include "ir.h"
#include "common.h"


#define FN_MAX_SIZE 1048576
#define BYTES_PER_LINE 32

/*** Parsing macros ***/
#define RD_U32()        read_u32leb(&buf)
#define RD_I32()        read_i32leb(&buf)
#define RD_U64()        read_u64leb(&buf)
#define RD_I64()        read_i64leb(&buf)
#define RD_NAME()       read_name(&buf)
#define RD_BYTESTR(len) read_bytes(&buf, len)

#define RD_BYTE()       read_u8(&buf)
#define RD_U32_RAW()    read_u32(&buf)
#define RD_U64_RAW()    read_u64(&buf)
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



/* Read LimitsType */
inline static wasm_limits_t read_limits(buffer_t &buf) {
  byte max_provided = RD_BYTE();
  uint32_t min = RD_U32();
  wasm_limits_t limits = {
    .initial = min,
    .max = (max_provided ? RD_U32() : 0),
    .has_max = max_provided
  };
  return limits;
}

/* Read MemoryType */
inline static MemoryDecl read_memtype(buffer_t &buf) {
  MemoryDecl mem = { .limits = read_limits(buf) };
  return mem;
}
/* Read TableType */
inline static TableDecl read_tabletype(buffer_t &buf) {
  byte reftype = RD_BYTE();
  if ((reftype != WASM_TYPE_FUNCREF) && (reftype != WASM_TYPE_EXTERNREF)) {
    throw std::runtime_error("Invalid Reftype\n");
  }
  TableDecl table = { .limits = read_limits(buf) };
  return table;
}

/* Read GlobalType */
inline static GlobalDecl read_globaltype(buffer_t &buf) {
  byte type = RD_BYTE();
  byte is_mutable = RD_BYTE();

  GlobalDecl glob = {
    .type = (wasm_type_t) type,
    .is_mutable = is_mutable
  };
  return glob;
}

inline static typelist read_type_list(uint32_t num, buffer_t &buf) {
  typelist vec;
  for (uint32_t j = 0; j < num; j++) {
    vec.push_back((wasm_type_t) RD_BYTE());
  }
  return vec;
}


/* Function to read const offset expression i32.const offset for elem/data */
static uint32_t decode_const_off_expr(buffer_t &buf) {
  // Has to be i32.const or i64.const offset for this
  byte opc = RD_BYTE();
  uint64_t offset = -1;
  switch (opc) {
    case WASM_OP_I32_CONST: {
      offset = RD_I32();
      break;
    }
    case WASM_OP_I64_CONST: {
      offset = RD_I64();
      break;
    }
    default:
      ERR("Unknown opcode in offset expr (%d): Must be i32/64 const\n", opc);
      throw std::runtime_error("Opcode error");
  }
  
  byte end = RD_BYTE();
  if (end != WASM_OP_END) {
    throw std::runtime_error("Malformed end in offset expr");
  }
  return offset;
}

// TODO: Init exprs
static void decode_init_expr(buffer_t &buf) {
  byte opcode = RD_BYTE();
  switch (opcode) {
    case WASM_OP_I32_CONST: {
      RD_I32();
      break;
    }
    case WASM_OP_I64_CONST: {
      int64_t v = RD_I64();
      break;
    }
    case WASM_OP_F32_CONST: {
      RD_U32_RAW();
      break;
    }
    case WASM_OP_F64_CONST: {
      RD_U64_RAW();
      break;
    }

    default:
      ERR("Unknown init expr opcode: %u\n", opcode);
      throw std::runtime_error("Opcode error");
  }

  byte end = RD_BYTE();
  if (end != WASM_OP_END) {
    throw std::runtime_error("Malformed end in init_expr");
  }

}


void WasmModule::decode_type_section(buffer_t &buf, uint32_t len) {
  uint32_t num_sigs = RD_U32();
  for (uint32_t i = 0; i < num_sigs; i++) {
    SigDecl sig;
    /* Read type */
    byte type = RD_BYTE();
    if (type != WASM_TYPE_FUNC) {
      throw std::runtime_error("Parse error: Signatures have to be func type!\n");
    }

    /* For params */
    uint32_t num_params = RD_U32();
    sig.params = read_type_list(num_params, buf);
    /* For results */
    uint32_t num_results = RD_U32();
    sig.results = read_type_list(num_results, buf);
    
    this->sigs.push_back(sig);
  }
}


void WasmModule::decode_import_section (buffer_t &buf, uint32_t len) {
  uint32_t num_imports = RD_U32();

  ImportInfo &info = this->imports;

  std::list <FuncDecl> &funcs = this->funcs;
  std::list <TableDecl> &tables = this->tables;
  std::list <MemoryDecl> &mems = this->mems;
  std::list <GlobalDecl> &globals = this->globals;

  for (uint32_t i = 0; i < num_imports; i++) {
    ImportDecl import;
    import.mod_name = RD_NAME();
    import.member_name = RD_NAME();
    import.kind = (wasm_kind_t) RD_BYTE();
    /* Populate the index space of respective kind */
    switch (import.kind) {
      case KIND_FUNC: {
        info.num_funcs++;
        uint32_t idx = RD_U32();
        FuncDecl func = {
          .sig = get_list_elem <SigDecl>(this->sigs, idx)
        };
        funcs.push_back(func);
        import.desc.func = &funcs.back();
        break;
      }
      case KIND_TABLE: {
        info.num_tables++;
        TableDecl table = read_tabletype(buf);
        tables.push_back(table);
        import.desc.table = &tables.back();
        break;
      }
      case KIND_MEMORY: {
        info.num_memories++;
        MemoryDecl mem = read_memtype(buf);
        mems.push_back(mem);
        import.desc.mem = &mems.back();
        break;
      }
      case KIND_GLOBAL: {
        info.num_globals++;
        GlobalDecl global = read_globaltype(buf);
        globals.push_back(global);
        import.desc.global= &globals.back();
        break;
      }
      default: {
        ERR("Import kind: %u\n", import.kind);
        throw std::runtime_error("Invalid import type");
      }
    }

    info.list.push_back(import);
  }

}

void WasmModule::decode_function_section (buffer_t &buf, uint32_t len) {
  uint32_t num_imports = this->imports.num_funcs;
  if (num_imports != this->funcs.size()) {
    throw std::runtime_error ("Malformed func imports");
  }

  uint32_t num_funcs = RD_U32();
  for (uint32_t i = 0; i < num_funcs; i++) {
    /* Get signature idx */
    uint32_t idx = RD_U32();
    FuncDecl func = {
      .sig = get_list_elem <SigDecl>(this->sigs, RD_U32())
    };
    this->funcs.push_back(func);
  }
}


void WasmModule::decode_table_section (buffer_t &buf, uint32_t len) {
  uint32_t num_imports = this->imports.num_tables;
  if (num_imports != this->tables.size()) {
    throw std::runtime_error ("Malformed table imports");
  }

  uint32_t num_tables = RD_U32();
  for (uint32_t i = 0; i < num_tables; i++) {
    TableDecl table = read_tabletype(buf);
    this->tables.push_back(table);
  }
}


void WasmModule::decode_memory_section (buffer_t &buf, uint32_t len) {
  uint32_t num_mems = RD_U32();
  MemoryDecl mem = read_memtype(buf);
  if ((num_mems == 1) && this->mems.empty()) {
    this->mems.push_back(mem);
  } else {
    throw std::runtime_error("Memory component has to be 1!");
  }
}


void WasmModule::decode_export_section (buffer_t &buf, uint32_t len) {
  uint32_t num_exports = RD_U32();

  /* String + exp descriptor + idx */
  for (uint32_t i = 0; i < num_exports; i++) {
    ExportDecl exp;
    exp.name = RD_NAME();
    exp.kind = (wasm_kind_t) RD_BYTE();
    uint32_t idx = RD_U32();
    /* Export descriptor */
    switch (exp.kind) {
      case KIND_FUNC:
        exp.desc.func = get_list_elem <FuncDecl>(this->funcs, idx);
        break;
      case KIND_TABLE:
        exp.desc.table = get_list_elem <TableDecl>(this->tables, idx);
        break;
      case KIND_MEMORY:
        exp.desc.mem = get_list_elem <MemoryDecl>(this->mems, idx);
        break;
      case KIND_GLOBAL:
        exp.desc.global = get_list_elem <GlobalDecl>(this->globals, idx);
        break;
      default: {
        ERR("Export kind: %u\n", exp.kind);
        throw std::runtime_error("Invalid export type");
      }
    }

    this->exports.push_back(exp);
  }
}


void WasmModule::decode_start_section (buffer_t &buf, uint32_t len) {
  this->has_start = true;
  this->start_idx = RD_U32();
  throw std::runtime_error("Start section not implemented");
}


void WasmModule::decode_element_section (buffer_t &buf, uint32_t len) {
  uint32_t num_elem = RD_U32();
  for (uint32_t i = 0; i < num_elem; i++) {
    ElemDecl elem;
    /* Flag */
    uint32_t flag = RD_U32();
    elem.flag = flag;
    /* Offset */
    uint32_t offset = -1;
    switch (flag) {
      case 0: offset = decode_const_off_expr(buf); break;
      default: {
        ERR("Invalid element segment flag: %d\n", flag);
        throw std::runtime_error("Flag error");
      }
    }
    elem.table_offset = offset;

    /* Element fn idx vector */
    uint32_t num_idxs = RD_U32();
    for (uint32_t i = 0; i < num_idxs; i++) {
      uint32_t fn_idx = RD_U32();
      FuncDecl* fptr = get_list_elem <FuncDecl>(this->funcs, fn_idx);
      elem.func_indices.push_back(fptr);
    }

    this->elems.push_back(elem);
  }
}


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


static wasm_localcsv_t decode_locals(buffer_t &buf) {
  /* Write num local elements */
  uint32_t num_localcse = RD_U32();
  
  uint32_t num_locals = 0;
  wasm_localcsv_t csv;
  for (uint32_t i = 0; i < num_localcse; i++) {
    uint32_t count = RD_U32();
    wasm_type_t type = (wasm_type_t) RD_BYTE();
    wasm_localcse_t cse = { .count = count, .type = type };
    csv.push_back(cse);
  }
  return csv;
}

/* Gets run after function section; in order */
void WasmModule::decode_code_section (buffer_t &buf, uint32_t len) {
  uint32_t num_fn = RD_U32();
  
  auto func_itr = std::next(this->funcs.begin(), this->imports.num_funcs);
  for (uint32_t i = 0; i < num_fn; i++) {
    FuncDecl &func = *func_itr;
    /* Fn size (locals + body) */
    uint32_t size = RD_U32();
    const byte* end_fn = buf.ptr + size;

    /* Local section */
    func.pure_locals = decode_locals(buf);
    /* TODO: Fn body expr */
    while (buf.ptr != end_fn) { RD_BYTE(); }
    std::advance (func_itr, 1);
  }
}


void WasmModule::decode_global_section(buffer_t &buf, uint32_t len) {
  uint32_t num_imports = this->imports.num_globals;
  if (num_imports != this->globals.size()) {
    throw std::runtime_error ("Malformed global imports");
  }

  uint32_t num_globs = RD_U32();
  for (uint32_t i = 0; i < num_globs; i++) {
    GlobalDecl global = read_globaltype(buf);
    decode_init_expr(buf);
    this->globals.push_back(global);
  }
}


void WasmModule::decode_data_section(buffer_t &buf, uint32_t len) {
  uint32_t num_datas = RD_U32();

  for (uint32_t i = 0; i < num_datas; i++) {
    DataDecl data;
    /* Flag */
    uint32_t flag = RD_U32();
    data.flag = flag;
    /* Offset */
    uint32_t offset = -1;
    switch (flag) {
      case 0: offset = decode_const_off_expr(buf); break;
      case 1: offset = 0; break;
      case 2: {
        uint32_t mem_idx = RD_U32();
        if (mem_idx != 0) throw std::runtime_error("Flag error: Only memory 0 allowed\n");
        offset = decode_const_off_expr(buf);
        break;
      }
      default: {
        ERR("Invalid data segment flag: %d\n", flag);
        throw std::runtime_error("Flag error");
      }
    }
    data.mem_offset = offset;
    /* Size val */
    uint32_t num_bytes = RD_U32();
    data.bytes = RD_BYTESTR(num_bytes);

    this->datas.push_back(data);
  }
}

void WasmModule::decode_datacount_section(buffer_t &buf, uint32_t len) {
  this->has_datacount = true;
  this->num_datas = RD_U32();
}

void WasmModule::decode_custom_section(buffer_t &buf, uint32_t len) {
  const byte* end_sec = buf.ptr + len;

  CustomDecl custom;
  custom.name = RD_NAME();
  uint32_t num_bytes = end_sec - buf.ptr;
  custom.bytes = RD_BYTESTR(num_bytes);

  this->customs.push_back(custom);
}



void decode_sections(WasmModule &module, buffer_t &buf) {

  while (buf.ptr < buf.end) {
    wasm_section_t section_id = (wasm_section_t) RD_BYTE();
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
    throw std::runtime_error("Empty bytecode");
  }

  /* Magic number & Version */
  uint32_t magic = RD_U32_RAW();
  if (magic != WASM_MAGIC) {
    throw std::runtime_error("Parse | Wasm Magic Value");
  }

  uint32_t version = RD_U32_RAW();
  if (version != WASM_VERSION) {
    throw std::runtime_error("Parse | Wasm Version");
  }

  module.version = version;

  decode_sections(module, buf);
  
  /* Has to match exactly */
  if (buf.ptr != buf.end) {
    throw std::runtime_error("Parse | Unexpected end");
  }
  return module;
}
