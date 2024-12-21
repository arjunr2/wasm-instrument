#include <sstream>

#include "routine_common.h"

#define INST(inv) InstBasePtr(new inv)

/* Method to look for either main or _start */
ExportDecl* get_main_export(WasmModule &module) {
  ExportDecl* main_fn_exp = module.find_export("main");
  ExportDecl* start_fn_exp = module.find_export("_start");
  if (!(main_fn_exp || start_fn_exp)) {
    throw std::runtime_error("Could not find export \"main\" or \"_start\" function!");
  }
  return main_fn_exp ? main_fn_exp : start_fn_exp;
}


/* Add pages to memory statically. Return the old number of pages */
uint32_t add_pages(MemoryDecl *mem, uint32_t num_pages) {
  wasm_limits_t &memlimit = mem->limits;
  uint32_t memdata_end = memlimit.initial * WASM_PAGE_SIZE;

  uint64_t retval = memlimit.initial;
  uint64_t new_size = retval + (uint64_t) num_pages;
  if (new_size > (1ULL<<32) - 1) {
    throw std::runtime_error("Adding pages exceeds 4GB limit");
  }
  if (memlimit.has_max && (new_size > memlimit.max)) {
    WARN("Adding more pages (%u + %u) than max memory size (%u); "
      "Expanding max accordingly\n", memlimit.initial, num_pages, 
      memlimit.max);
  }
  memlimit.initial = (uint32_t) new_size;
  return retval;
}


// Core instrumentation for call_indirect interposition
// Update elems with funcref (works only if module uses active elems):
// `call_indirect`s will go through our instrumented elem targets, allowing us
// interpose on its capability dynamically.
// Can pass an additional parameter for marshalling data
std::set<FuncDecl*> instrument_funcref_elems (WasmModule &module, 
      std::vector<wasm_type_t> marshall_vals) {
    uint32_t funcref_counter = 0;
    std::set<FuncDecl*> newfuncs;
    for (auto &elem: module.Elems()) {
        if (elem.flag != 0) {
            ERR("Currently only active elems are supported (got flag: %d)\n", elem.flag);
        }
        for (auto &funcref: elem.funcs) {
            std::ostringstream oss;
            auto dname_pair = module.get_debug_name_from_func(funcref);
            if (dname_pair.first) {
                oss << dname_pair.second << "__funcref_wrapper";
            } else {
                oss << funcref_counter << "__funcref_wrapper";
            }

            SigDecl new_fn_sig(*(funcref->sig));
            for (auto &val: marshall_vals) {
              // Marshalled values appear after the call parameters
              new_fn_sig.params.push_back(val);
            }
            FuncDecl new_fn_decl = {
                .sig = module.add_sig(new_fn_sig, false),
                .pure_locals = wasm_localcsv_t(),
                .num_pure_locals = 0,
                .instructions = {
                    INST(EndInst())
                }
            };

            std::string new_dname = oss.str();
            FuncDecl *new_fn = module.add_func(new_fn_decl, NULL, new_dname.c_str());
            InstBuilder param_builder = {};
            for (int i = 0; i < funcref->sig->params.size(); i++) {
                param_builder.push_inst(LocalGetInst(i));
            }
            param_builder.push_inst(CallInst(funcref));
            param_builder.splice_into(new_fn->instructions, new_fn->instructions.begin());

            // Replace the original funcref in elem section with the new function
            funcref = new_fn;
            newfuncs.insert(new_fn);

            funcref_counter++;
        }
    }
    return newfuncs;
}