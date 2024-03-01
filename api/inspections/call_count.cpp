#include "inspect_common.h"
#include "cJSON.h"


std::string call_count_inspect(WasmModule &module) {
  cJSON *json_out = cJSON_CreateObject();
  std::list<DebugNameAssoc> *debug = module.getFnDebugNames();
  for (auto &entry : *debug) {
    int idx = module.getFuncIdx(entry.func);
    if (!module.isImport(entry.func)) {
      uint32_t calls = 0;
      uint32_t indirects = 0;
      uint32_t num_inst = 0;
      for (auto &inst: entry.func->instructions) {
        switch(inst->getOpcode()) {
          case WASM_OP_CALL_INDIRECT:
          case WASM_OP_CALL_REF:
                      indirects++;
          case WASM_OP_CALL:
                      calls++;
                      break;
          default:    {};
        }
        num_inst++;
      }
      cJSON *js = cJSON_CreateObject();
      cJSON_AddNumberToObject(js, "calls", calls);
      cJSON_AddNumberToObject(js, "indirects", indirects);
      cJSON_AddNumberToObject(js, "inst_count", num_inst);
      cJSON_AddItemToObject(json_out, entry.name.c_str(), js);
    }
  }
  std::string ret = cJSON_Print(json_out);
  cJSON_Delete(json_out);
  return ret;
}
