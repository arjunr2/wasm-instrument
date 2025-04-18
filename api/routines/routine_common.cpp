#include <sstream>

#include "routine_common.h"

#define INST(inv) InstBasePtr(new inv)

// Instructions to typecast any type to I64
InstBuilder builder_push_i64_extend(InstBuilder &builder, wasm_type_t type) {
    switch (type) {
        case WASM_TYPE_I32: builder.push_inst (I64ExtendI32UInst()); break;
        case WASM_TYPE_I64: ; break;
        case WASM_TYPE_F32: builder.push({
            INST (I32ReinterpretF32Inst()),
            INST (I64ExtendI32UInst())
        }); break;
        case WASM_TYPE_F64: builder.push_inst (I64ReinterpretF64Inst()); break;
		default: { 
            ERR("Error: Unsupported type %d for I64 Extend\n", type); 
        }
    }
    return builder;
}


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
    WARN("Adding more pages (%u + %u) than current max memory size (%u); "
      "Expanding max accordingly\n", memlimit.initial, num_pages, 
      memlimit.max);
    memlimit.max = (uint32_t) new_size;
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


// Method to insert instructions at the end of execution of entry function or relevant import funcs
void insert_on_exit(WasmModule &module, FuncDecl *entry_func, InstBuilder &builder, 
    std::vector<ImportInfo> import_func_exits) {

  auto instbuf = builder.get_buf();

  std::set<FuncDecl*> exit_decls;
  for (auto &x: import_func_exits) {
    exit_decls.insert(module.find_import_func(x.mod_name, x.member_name));
  }

  // Probe the calls to an exit decl
  if (exit_decls.size() > 0) {
    instrument_funcref_elems(module, {});
    for (auto &func: module.Funcs()) {
      for (auto institr = func.instructions.begin(); institr != func.instructions.end(); ++institr) {
        InstBasePtr &instptr = *institr;
        if (instptr->getImmType() == IMM_FUNC) {
          auto immfunc = static_pointer_cast<ImmFuncInst>(instptr);
          if (exit_decls.contains(immfunc->getFunc())) {
            func.instructions.insert(institr, instbuf.begin(), instbuf.end());
          }
        }
      }
    }
  }

  // Probe entry function's return
  for (auto institr = entry_func->instructions.begin(); institr != entry_func->instructions.end(); ++institr) {
      InstBasePtr instptr = *institr;
      uint16_t opcode = instptr->getOpcode();
      if (opcode == WASM_OP_RETURN) {
        entry_func->instructions.insert(institr, instbuf.begin(), instbuf.end());
      }
  }
  // Probe entry function end
  auto enditr = std::prev(entry_func->instructions.end());
  entry_func->instructions.insert(enditr, instbuf.begin(), instbuf.end());
}

