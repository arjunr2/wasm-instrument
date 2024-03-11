#include <stdexcept>
#include <iostream>

#include "ir.h"
#include "common.h"
#include "inst_internal.h"


#define FN_MAX_SIZE 1048576
#define BYTES_PER_LINE 32


/* Read LimitsType */
inline static wasm_limits_t read_limits(buffer_t &buf) {
  byte lb = RD_BYTE();
  byte max_provided = lb & 0x01;
  byte flag = (lb >> 1);

  uint32_t min = RD_U32();
  wasm_limits_t limits = {
    .initial = min,
    .max = (max_provided ? RD_U32() : 0),
    .has_max = max_provided,
    .flag = flag
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
  TableDecl table = { .reftype = (wasm_type_t) reftype, .limits = read_limits(buf) };
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
static uint32_t decode_const_off_expr(buffer_t &buf, Opcode_t &opcode) {
  // Has to be i32.const or i64.const offset for this
  Opcode_t opc = RD_OPCODE();
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
  
  Opcode_t end = RD_OPCODE();
  if (end != WASM_OP_END) {
    throw std::runtime_error("Malformed end in offset expr");
  }
  opcode = opc;
  return offset;
}

// TODO: Init exprs (just return bytes right now)
static bytearr decode_init_expr(buffer_t &buf) {
  const byte* startp = buf.ptr;
  Opcode_t opcode = RD_OPCODE();
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

  Opcode_t end = RD_OPCODE();
  if (end != WASM_OP_END) {
    throw std::runtime_error("Malformed end in init_expr");
  }
  const byte* endp = buf.ptr;
  return bytearr(startp, endp);
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

  auto &info = this->imports;

  auto &funcs = this->funcs;
  auto &tables = this->tables;
  auto &mems = this->mems;
  auto &globals = this->globals;

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
        FuncDecl func = { .sig = this->getSig(idx) };
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
        info.num_mems++;
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
      .sig = this->getSig(idx)
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
  if ((num_mems == 1) && this->mems.empty()) {
    MemoryDecl mem = read_memtype(buf);
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
        exp.desc.func = this->getFunc(idx);
        break;
      case KIND_TABLE:
        exp.desc.table = this->getTable(idx);
        break;
      case KIND_MEMORY:
        exp.desc.mem = this->getMemory(idx);
        break;
      case KIND_GLOBAL:
        exp.desc.global = this->getGlobal(idx);
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
  this->start_fn = this->getFunc(RD_U32());
  TRACE("NOTE: Start section not fully complete\n");
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
      case 0: offset = decode_const_off_expr(buf, elem.opcode_offset); break;
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
      auto fptr = this->getFunc(fn_idx);
      elem.func_indices.push_back(fptr);
    }

    this->elems.push_back(elem);
  }
}




static wasm_localcsv_t decode_locals(buffer_t &buf, uint32_t &num_locals) {
  /* Write num local elements */
  uint32_t num_localcse = RD_U32();
  
  uint32_t total_ct = 0;
  wasm_localcsv_t csv;
  for (uint32_t i = 0; i < num_localcse; i++) {
    uint32_t count = RD_U32();
    wasm_type_t type = (wasm_type_t) RD_BYTE();
    wasm_localcse_t cse = { .count = count, .type = type };
    csv.push_back(cse);
    total_ct += count;
  }
  num_locals = total_ct;
  return csv;
}



/* Code decoding for instructions: Calls internal instructions */
InstList WasmModule::decode_expr_to_insts (buffer_t &buf, bool gen_cfg) {
  InstList ilist = { };
  while (buf.ptr < buf.end) {
    Opcode_t opcode = RD_OPCODE();

    opcode_imm_type imm_type = opcode_table[opcode].imm_type;
    TRACE("O: %s\n", opcode_table[opcode].mnemonic);

    InstBasePtr instptr;
    #define ICS(cs, clsname) \
      case cs:  \
        instptr.reset(new clsname((*this), opcode, buf)); break;
    switch (imm_type) {
      ICS (IMM_NONE, ImmNoneInst);
      ICS (IMM_BLOCKT, ImmBlocktInst);
      ICS (IMM_LABEL, ImmLabelInst);
      ICS (IMM_LABELS, ImmLabelsInst);
      ICS (IMM_FUNC, ImmFuncInst);
      ICS (IMM_SIG_TABLE, ImmSigTableInst);
      ICS (IMM_LOCAL, ImmLocalInst);
      ICS (IMM_GLOBAL, ImmGlobalInst);
      ICS (IMM_TABLE, ImmTableInst);
      ICS (IMM_MEMARG, ImmMemargInst);
      ICS (IMM_I32, ImmI32Inst);
      ICS (IMM_F64, ImmF64Inst);
      ICS (IMM_MEMORY, ImmMemoryInst);
      ICS (IMM_TAG, ImmTagInst);
      ICS (IMM_I64, ImmI64Inst);
      ICS (IMM_F32, ImmF32Inst);
      ICS (IMM_REFNULLT, ImmRefnulltInst);
      ICS (IMM_VALTS, ImmValtsInst);
      // Extension Immediates
      ICS (IMM_DATA_MEMORY, ImmDataMemoryInst);
      ICS (IMM_DATA, ImmDataInst);
      ICS (IMM_MEMORYCP, ImmMemorycpInst);
      ICS (IMM_DATA_TABLE, ImmDataTableInst);
      ICS (IMM_TABLECP, ImmTablecpInst);
      ICS (IMM_V128, ImmV128Inst);
      ICS (IMM_LANEIDX, ImmLaneidxInst);
      ICS (IMM_LANEIDX16, ImmLaneidx16Inst);
      ICS (IMM_MEMARG_LANEIDX, ImmMemargLaneidxInst);
      default:
        ERR("Unknown imm type: %d\n", imm_type);
        throw std::runtime_error("Unknown imm");
    }
    ilist.push_back(instptr);
  }
  if (buf.ptr != buf.end) {
    throw std::runtime_error("Unaligned end for instruction parsing\n");
  }
  TRACE("=== Function Parsed Successfully ===\n");
  return ilist;
}


/* Gets run after function section; in order */
void WasmModule::decode_code_section (buffer_t &buf, uint32_t len, bool gen_cfg) {
  uint32_t num_fn = RD_U32();
  
  uint32_t num_imports = this->imports.num_funcs;
  auto &funcs = this->funcs;
  for (auto func_itr = std::next(funcs.begin(), num_imports);
          func_itr != funcs.end(); ++func_itr) {
    FuncDecl &func = *func_itr;
    /* Fn size (locals + body) */
    uint32_t size = RD_U32();
    const byte* end_insts = buf.ptr + size;

    /* Local section */
    func.pure_locals = decode_locals(buf, func.num_pure_locals);
    
    const byte* start_insts = buf.ptr;
    /* Fn body bytes and expr */
    func.code_bytes = RD_BYTESTR(end_insts - start_insts);
    buffer_t cbuf = { start_insts, start_insts, end_insts };
    func.instructions = this->decode_expr_to_insts(cbuf, gen_cfg);
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
    global.init_expr_bytes = decode_init_expr(buf);
    this->globals.push_back(global);
  }
}


void WasmModule::decode_data_section(buffer_t &buf, uint32_t len) {
  uint32_t num_datas = RD_U32();
  if (this->has_datacount) {
    if (num_datas != this->num_datas_datacount) {
      ERR("Num datas: %u | Num datacount: %u\n", num_datas, this->num_datas_datacount);
      throw std::runtime_error ("Datacount section and data section vector "
                                "size don't match");
    }
  }
  /* Create datas if datacount wasn't present to do so */
  else {
    for (int i = 0; i < num_datas; i++)
      this->datas.push_back(DataDecl());
  }


  for (auto &data : this->datas) {
    /* Flag */
    uint32_t flag = RD_U32();
    data.flag = flag;
    /* Offset */
    uint32_t offset = -1;
    /* Memory */
    MemoryDecl *mem = NULL;
    switch (flag) {
      case 0: {
        offset = decode_const_off_expr(buf, data.opcode_offset);
        mem = this->getMemory(0);
        break;
      }
      case 1: offset = 0; break;
      case 2: {
        uint32_t mem_idx = RD_U32();
        offset = decode_const_off_expr(buf, data.opcode_offset);
        mem = this->getMemory(mem_idx);
        break;
      }
      default: {
        ERR("Invalid data segment flag: %d\n", flag);
        throw std::runtime_error("Flag error");
      }
    }
    data.mem_offset = offset;
    data.mem = mem;
    /* Size val */
    uint32_t num_bytes = RD_U32();
    data.bytes = RD_BYTESTR(num_bytes);
  }
}

void WasmModule::decode_datacount_section(buffer_t &buf, uint32_t len) {
  this->has_datacount = true;
  uint32_t num_datas = RD_U32();
  this->num_datas_datacount = num_datas;
  /* Create data list so instructions in code can decode accessors correctly */
  for (int i = 0; i < num_datas; i++) {
    this->datas.push_back(DataDecl());
  }
}

void WasmModule::decode_custom_section(buffer_t &buf, uint32_t len) {
  const byte* end_sec = buf.ptr + len;

  CustomDecl custom;
  custom.name = RD_NAME();
  uint32_t num_bytes = end_sec - buf.ptr;

  if (custom.name == "name") {
    DebugNameDecl &debug = custom.debug;
    while (buf.ptr != end_sec) {
      byte id = RD_BYTE();
      uint32_t len = RD_U32();
      /* Non-function subsections in name: Just record section info */
      if (id != 1) {
        SubsecBytes subsec = { .id = id, .bytes = RD_BYTESTR(len) };
        debug.subsections.push_back(subsec);
      }
      /* Function subsection */
      else {
        const byte* start_subsec = buf.ptr;
        const byte* end_subsec = buf.ptr + len;

        uint32_t num_names = RD_U32();
        for (uint32_t i = 0; i < num_names; i++) {
          uint32_t idx = RD_U32();
          DebugNameAssoc d = { .func = this->getFunc(idx), .name = RD_NAME() };
          debug.func_assoc.push_back(d);
        }

        if (buf.ptr != end_subsec) {
          ERR("Custom name section not aligned after parsing -- start:%lu, ptr:%lu, end:%lu\n",
              buf.ptr - start_subsec,
              buf.ptr - start_subsec,
              buf.end - start_subsec);
          throw std::runtime_error("Subsection parsing error");
        }
      }
    }
  }
  /* Non-name sections: Just get bytes */
  else {
    custom.bytes = RD_BYTESTR(num_bytes);
  }

  this->customs.push_back(custom);
  if (custom.name == "name") {
    this->fn_names_debug = &this->customs.back().debug.func_assoc;
  }
}


/* Wasm Module parser from buffer bytecode */
void WasmModule::decode_buffer(buffer_t &buf, bool gen_cfg) {

  /* Magic number & Version */
  uint32_t magic = RD_U32_RAW();
  if (magic != WASM_MAGIC) {
    throw std::runtime_error("Parse | Wasm Magic Value");
  }

  uint32_t version = RD_U32_RAW();
  if (version != WASM_VERSION) {
    throw std::runtime_error("Parse | Wasm Version");
  }

  this->magic = magic;
  this->version = version;

  /* Decode sections */
  while (buf.ptr < buf.end) {
    wasm_section_t section_id = (wasm_section_t) RD_BYTE();
    uint32_t len = RD_U32();

    TRACE("Found section \"%s\", len: %d\n", wasm_section_name(section_id), len);

    buffer_t cbuf = {buf.ptr, buf.ptr, buf.ptr + len};
    #define DECODE_CALL(sec,...)  this->decode_##sec##_section (cbuf, len __VA_OPT__(,) __VA_ARGS__); break;
    switch (section_id) {
      case WASM_SECT_TYPE:      DECODE_CALL(type); 
      case WASM_SECT_IMPORT:    DECODE_CALL(import); 
      case WASM_SECT_FUNCTION:  DECODE_CALL(function); 
      case WASM_SECT_TABLE:     DECODE_CALL(table); 
      case WASM_SECT_MEMORY:    DECODE_CALL(memory); 
      case WASM_SECT_GLOBAL:    DECODE_CALL(global); 
      case WASM_SECT_EXPORT:    DECODE_CALL(export); 
      case WASM_SECT_START:     DECODE_CALL(start); 
      case WASM_SECT_ELEMENT:   DECODE_CALL(element);  
      case WASM_SECT_CODE:      DECODE_CALL(code, gen_cfg); 
      case WASM_SECT_DATA:      DECODE_CALL(data); 
      case WASM_SECT_DATACOUNT: DECODE_CALL(datacount); 
      case WASM_SECT_CUSTOM:    DECODE_CALL(custom); 
      default:
        ERR("Unknown section id: %u\n", section_id);
        throw std::runtime_error("Section parsing error");
    }

    if (cbuf.ptr != cbuf.end) {
      ERR("Section \"%s\" not aligned after parsing -- start:%lu, ptr:%lu, end:%lu\n", 
          wasm_section_name(section_id),
          cbuf.start - buf.start, 
          cbuf.ptr - buf.start, 
          cbuf.end - buf.start);
      throw std::runtime_error("Section parsing error");
    }
    // Advance section
    buf.ptr = cbuf.ptr;
  }
}


/* Main Parse routine */
WasmModule parse_bytecode(const byte* start, const byte* end, bool gen_cfg = true) {
  
  WasmModule module = {};

  /* Initialize buffer */
  buffer_t main_buf = {start, start, end};
  buffer_t &buf = main_buf;

  if (buf.ptr == buf.end) { 
    throw std::runtime_error("Empty bytecode");
  }

  /* Decode entire module elements */
  module.decode_buffer(buf, gen_cfg);
  
  /* Has to match exactly */
  if (buf.ptr != buf.end) {
    throw std::runtime_error("Parse | Unexpected end");
  }
  return module;
}
