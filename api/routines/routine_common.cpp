#include "routine_common.h"

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
