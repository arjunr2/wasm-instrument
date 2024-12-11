#include "inspect_common.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
std::string func_indexes_inspect(WasmModule &module) {
  json outd = {{"imports", json::object()}, {"defs", json::object()}};
  std::list<DebugNameAssoc> *debug = module.getFnDebugNames();
  for (auto &entry : *debug) {
    int idx = module.getFuncIdx(entry.func);
    if (module.isImport(entry.func)) {
      outd["imports"][entry.name] = idx;
    } else {
      outd["defs"][entry.name] = idx;
    }
  }
  std::string ret = outd.dump();
  return ret;
}
