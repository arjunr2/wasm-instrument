#include "views.h"

#define ADD_SCOPE(scopeblock) ({  \
  static_scopes.push_back(scopeblock);  \
  dynamic_scopes.push_back(&static_scopes.back()); \
})

#define REMOVE_SCOPE(enditr) ({ \
  ScopeBlock* s = dynamic_scopes.back();  \
  s->set_end(enditr); \
  dynamic_scopes.pop_back();  \
})

ScopeList WasmModule::gen_scopes_from_instructions(FuncDecl *func) {
  ScopeList static_scopes;
  if (this->isImport(func)) {
    return static_scopes;
  }

  std::list<ScopeBlock*> dynamic_scopes;

  InstList &instructions = func->instructions;

  /* First instruction starts scope */
  ADD_SCOPE(ScopeBlock(instructions.begin()));

  auto last_institr = instructions.begin();
  for (auto institr = instructions.begin(); institr != instructions.end(); ++institr) {
    InstBasePtr inst = *institr;
    byte opcode = inst->getOpcode();
    switch (opcode) {
      /* Scope begin instructions */
      case WASM_OP_LOOP:
      case WASM_OP_BLOCK:
      case WASM_OP_IF: {
          ADD_SCOPE(ScopeBlock(institr));
          break;
      }
      /* Scope end instructions */
      case WASM_OP_END: {
          REMOVE_SCOPE(institr);
          break;
      }
      /* Scope end + start instructions */
      case WASM_OP_ELSE: {
          REMOVE_SCOPE(last_institr);
          ADD_SCOPE(institr);
          break;
      }
      default: {}
    }
    last_institr = institr; 
  }
  
  return static_scopes;
}
