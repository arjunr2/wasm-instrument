#include "routine_common.h"

void func_entry_instrument(WasmModule &module) {
    uint64_t num_funcs = 0;

    ImportInfo iminfo = {.mod_name = "instrument", .member_name = "logfunc"};
    SigDecl logfunc_sig = {.params = {WASM_TYPE_I32}, .results = {}};
    ImportDecl *logfunc_import_decl = module.add_import(iminfo, logfunc_sig);
    FuncDecl *logfunc_import = logfunc_import_decl->desc.func;

    for (auto &func : module.Funcs()) {
        InstList &insts = func.instructions;
        InstList addinst;
        addinst.push_back(InstBasePtr(new I32ConstInst(num_funcs++)));
        addinst.push_back(InstBasePtr(new CallInst(logfunc_import)));
        insts.insert(insts.begin(), addinst.begin(), addinst.end());
    }
}
