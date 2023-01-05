#include <iostream>
#include "encode.h"
#include "ir.h"

typedef enum {
  custom_id = 0,
  type_id,
  import_id,
  function_id,
  table_id,
  memory_id,
  global_id,
  export_id,
  start_id,
  element_id,
  code_id,
  data_id,
  datacount_id
} section_id;

/* Write LimitsType */
inline static void write_limits(bytedeque &bdeq, wasm_limits_t &limits) {
  WR_BYTE (limits.has_max);
  WR_U32 (limits.initial);
  if (limits.has_max) {
    WR_U32 (limits.max);
  }
}

/* Write MemoryType */
inline static void write_memtype(bytedeque &bdeq, MemoryDecl &mem) {
  write_limits (bdeq, mem.limits);
}

/* Write TableType */
inline static void write_tabletype(bytedeque &bdeq, TableDecl &table) {
  WR_BYTE (table.reftype);
  write_limits (bdeq, table.limits);
}

/* Write GlobalType */
inline static void write_globaltype(bytedeque &bdeq, GlobalDecl &global) {
  WR_BYTE (global.type);
  WR_BYTE (global.is_mutable);
}

void write_type_list (bytedeque &bdeq, typelist &tlist) {
  WR_U32(tlist.size());
  for (auto &type : tlist)
    WR_BYTE(type);
}

bytedeque WasmModule::encode_type_section() {
  bytedeque bdeq;

  WR_U32 (this->sigs.size());
  for (auto &sig : this->sigs) {
    WR_BYTE(WASM_TYPE_FUNC);
    /* For params */
    write_type_list (bdeq, sig.params);
    write_type_list (bdeq, sig.results);
  }
 
  return bdeq;
}

bytedeque WasmModule::encode_import_section() {
  bytedeque bdeq;

  ImportInfo &imports = this->imports;
  uint32_t total_imports = imports.get_num_imports();
  if (imports.list.size() != total_imports) {
    throw std::runtime_error ("Import list size and counts don't match\n");
  }

  WR_U32(total_imports);
  for (auto &import : imports.list) {
    WR_NAME (import.mod_name);
    WR_NAME (import.member_name);
    WR_BYTE (import.kind);
    switch(import.kind) {
      case KIND_FUNC: {
        uint32_t idx = GET_LIST_IDX(this->sigs, import.desc.func->sig);
        WR_U32 (idx);
        break;
      }
      case KIND_TABLE: {
        write_tabletype(bdeq, *import.desc.table);
        break;
      }
      case KIND_MEMORY: {
        write_memtype(bdeq, *import.desc.mem);
        break;
      }
      case KIND_GLOBAL: {
        write_globaltype(bdeq, *import.desc.global);
        break;
      }
      default:
        ERR ("Unknown Import Kind: %d | %d\n", 
          GET_LIST_IDX(imports.list, &import), import.kind);
        throw std::runtime_error ("Unknown Kind");
    }
  }
  
  return bdeq;
}

bytedeque WasmModule::encode_function_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_table_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_memory_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_global_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_export_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_start_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_element_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_code_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_data_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_datacount_section() {
  bytedeque bdeq;
  return bdeq;
}

bytedeque WasmModule::encode_custom_section() {
  bytedeque bdeq;
  return bdeq;
}


static void dump_bytedeque(bytedeque &bdeq) {
  uint32_t i = 0;
  for (auto &it: bdeq) {
    printf("%02x ", it);
    i++;
    if (!(i & 15)) { printf("\n"); }
  }
  printf("\n");
}


/* Main Encoder routine */
bytedeque WasmModule::encode_module() {
  bytedeque bdeq;

  WR_U32_RAW (this->magic);
  WR_U32_RAW (this->version);

  #define ENCODE_CALL(sec) { \
    auto secdeq = this->encode_##sec##_section();  \
    uint32_t section_size = secdeq.size();  \
    /* Push section information */  \
    if (section_size) { \
      WR_SECLEN_PRE ();  \
      WR_SECBYTE_PRE (sec##_id); \
      bdeq.insert(bdeq.end(), secdeq.begin(), secdeq.end()); \
    } \
  }
  ENCODE_CALL(custom); 
  ENCODE_CALL(type); 
  ENCODE_CALL(import); 
  ENCODE_CALL(function); 
  ENCODE_CALL(table); 
  ENCODE_CALL(memory); 
  ENCODE_CALL(global); 
  ENCODE_CALL(export); 
  ENCODE_CALL(start); 
  ENCODE_CALL(element);  
  ENCODE_CALL(code); 
  ENCODE_CALL(data); 
  ENCODE_CALL(datacount); 

  dump_bytedeque(bdeq);
  return bdeq;
}
