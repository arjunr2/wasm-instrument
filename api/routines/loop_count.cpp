#include "routine_common.h"

void loop_instrument (WasmModule &module) {
  uint64_t num_loops = 0;
  for (auto &func : module.Funcs()) {
    InstList &insts = func.instructions;
    for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
      InstBasePtr &instruction = *institr;
      // Increment global after loop
      if (instruction->is(WASM_OP_LOOP)) {
        GlobalDecl global = {
          .type = WASM_TYPE_I64, 
          .is_mutable = true,
          .init_expr_bytes = INIT_EXPR (I64_CONST, 0)
        };
        GlobalDecl *gref = module.add_global(global, 
                              (std::string("__lc_") + std::to_string(num_loops++)).c_str());
        auto loc = std::next(institr);
        InstList addinst;
        addinst.push_back(InstBasePtr(new GlobalGetInst(gref)));
        addinst.push_back(InstBasePtr(new I64ConstInst(1)));
        addinst.push_back(InstBasePtr(new I64AddInst()));
        addinst.push_back(InstBasePtr(new GlobalSetInst(gref)));
        insts.insert(loc, addinst.begin(), addinst.end());
      }
    }
  }
}
