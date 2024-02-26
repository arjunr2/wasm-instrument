#include "inspect_common.h"
#include "cJSON.h"


std::string func_indexes_inspect(WasmModule &module) {
  cJSON *imports_js = cJSON_CreateObject();
  cJSON *defs_js = cJSON_CreateObject();
  std::list<DebugNameAssoc> *debug = module.getFnDebugNames();
  for (auto &entry : *debug) {
    int idx = module.getFuncIdx(entry.func);
    if (module.isImport(entry.func)) {
      cJSON_AddNumberToObject(imports_js, entry.name.c_str(), idx);
    } else {
      cJSON_AddNumberToObject(defs_js, entry.name.c_str(), idx);
    }
  }

  cJSON *json_out = cJSON_CreateObject();
  cJSON_AddItemToObject(json_out, "imports", imports_js);
  cJSON_AddItemToObject(json_out, "defs", defs_js);
  std::string ret = cJSON_Print(json_out);
  cJSON_Delete(json_out);
  return ret;
}
