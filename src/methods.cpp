#include "builder.h"
#include "instructions.h"
#include "ir.h"
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

// Look for either 'main' or '_start' function
FuncDecl *WasmModule::get_main_fn() {
    ExportDecl *main_fn_exp = this->find_export("main");
    ExportDecl *start_fn_exp = this->find_export("_start");
    if (!((main_fn_exp && main_fn_exp->kind == KIND_FUNC) ||
          (start_fn_exp && start_fn_exp->kind == KIND_FUNC))) {
        WARN("Could not find export \"main\" or \"_start\" function!");
        return NULL;
    }
    // If both are present, prefer '_start'
    return start_fn_exp ? start_fn_exp->desc.func : main_fn_exp->desc.func;
}

// Instrumentation to generate wrappers on funcref elements in tables  (works
// only on active elems) Critical component to enable 'call_indirect'
// instrumentation. Wrapper can be instrumented to dynamically capture indirect
// calls. Extra contextual data can be marshalled with 'marshall_vals' to the
// wrapper Returns set of added wrapper functions
std::set<FuncDecl *>
WasmModule::funcref_wrap(std::vector<wasm_type_t> marshall_vals) {
    uint32_t funcref_counter = 0;
    std::set<FuncDecl *> newfuncs = {};
    for (auto &elem : this->Elems()) {
        if (elem.flag != 0) {
            ERR("Currently only active elems are supported (got flag: %d)\n",
                elem.flag);
        }
        for (auto &funcref : elem.funcs) {
            std::ostringstream oss;
            auto dname_pair = this->get_debug_name_from_func(funcref);
            if (dname_pair.first) {
                oss << dname_pair.second << "__funcref_wrapper";
            } else {
                oss << funcref_counter << "__funcref_wrapper";
            }

            SigDecl new_fn_sig(*(funcref->sig));
            for (auto &val : marshall_vals) {
                // Marshalled values appear after the call parameters
                new_fn_sig.params.push_back(val);
            }
            FuncDecl new_fn_decl = {.sig = this->add_sig(new_fn_sig, false),
                                    .pure_locals = wasm_localcsv_t(),
                                    .num_pure_locals = 0,
                                    .instructions = {INST(EndInst())}};

            std::string new_dname = oss.str();
            FuncDecl *new_fn =
                this->add_func(new_fn_decl, NULL, new_dname.c_str());
            InstBuilder param_builder = {};
            for (int i = 0; i < funcref->sig->params.size(); i++) {
                param_builder.push_inst(LocalGetInst(i));
            }
            param_builder.push_inst(CallInst(funcref));
            param_builder.splice_into(new_fn->instructions,
                                      new_fn->instructions.begin());

            // Replace the original funcref in elem section with the new
            // function
            funcref = new_fn;
            newfuncs.insert(new_fn);

            funcref_counter++;
        }
    }
    return newfuncs;
}

// Append instructions at all points before a module exits (end of entry
// functions) Can include an list of import functions as "exit points" NOTE:
// Implicitly transforms indirect calls to direct calls
void WasmModule::exit_instrument(InstBuilder &builder,
                                 std::vector<ImportInfo> import_func_exits,
                                 FuncDecl *entry_func) {
    auto instbuf = builder.get_buf();

    std::set<FuncDecl *> exit_decls = {};
    for (auto &x : import_func_exits) {
        exit_decls.insert(this->find_import_func(x.mod_name, x.member_name));
    }

    // Probe the calls to an exit decl
    if (exit_decls.size() > 0) {
        this->funcref_wrap({});
        for (auto &func : this->Funcs()) {
            for (auto institr = func.instructions.begin();
                 institr != func.instructions.end(); ++institr) {
                InstBasePtr &instptr = *institr;
                if (instptr->getImmType() == IMM_FUNC) {
                    auto immfunc = static_pointer_cast<ImmFuncInst>(instptr);
                    if (exit_decls.contains(immfunc->getFunc())) {
                        func.instructions.insert(institr, instbuf.begin(),
                                                 instbuf.end());
                    }
                }
            }
        }
    }

    // Probe entry function's return
    if (entry_func == NULL) {
        entry_func = this->get_main_fn();
        if (!entry_func) {
            WARN("Missing an entry function for exit instrumentation\n");
            return;
        }
    }
    for (auto institr = entry_func->instructions.begin();
         institr != entry_func->instructions.end(); ++institr) {
        InstBasePtr instptr = *institr;
        uint16_t opcode = instptr->getOpcode();
        if (opcode == WASM_OP_RETURN) {
            entry_func->instructions.insert(institr, instbuf.begin(),
                                            instbuf.end());
        }
    }
    // Probe entry function end
    auto enditr = std::prev(entry_func->instructions.end());
    entry_func->instructions.insert(enditr, instbuf.begin(), instbuf.end());
}

void WasmModule::beforeImmTypeInstruction(opcode_imm_type imm_type,
                                          InstructionSiteCallback callback) {
    for (auto &func : this->Funcs()) {
        for (auto institr = func.instructions.begin();
             institr != func.instructions.end(); ++institr) {
            InstBasePtr &instptr = *institr;
            if (instptr->getImmType() == imm_type) {
                callback(*this, &func, instptr);
            }
        }
    }
}

// Instrument before a direct function call
// NOTE: To support indirect calls, call 'funcref_wrap' before
void WasmModule::beforeFuncCall(
    std::map<FuncDecl *, DirectCallSiteCallback> &builder_map) {
    auto x = [&builder_map](WasmModule &module, FuncDecl *func,
                            InstBasePtr instptr) {
        auto call_func = static_pointer_cast<ImmFuncInst>(instptr)->getFunc();
        if (builder_map.contains(call_func)) {
            builder_map[call_func](module, func, call_func);
        }
    };
    this->beforeImmTypeInstruction(IMM_FUNC, x);
}
