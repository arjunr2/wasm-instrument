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
  uint32_t total_imports = imports.num_funcs + imports.num_tables + 
                            imports.num_memories + imports.num_globals;
  WR_U32(total_imports);
  
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
    printf("%02X ", it);
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
