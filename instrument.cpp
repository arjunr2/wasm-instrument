#include "ir.h"

/*** Instrumentation Functions ***/
/* Add a global to a module */
GlobalDecl& WasmModule::add_global (GlobalDecl &global) {
  this->globals.push_back(global);
  return this->globals.back();
}

/* Imports for all declarations */
#define IMPORT_INST(kd) \
  ImportDecl import = { \
    .mod_name = info.mod_name,  \
    .member_name = info.member_name, \
    .kind = kd  \
  };

#define IMPORT_ADD(field, decl) { \
  auto &field##s = this->field##s;  \
  auto &imports = this->imports;  \
  /* Add to import list and count */  \
  import.desc.field = &field##s.front();  \
  imports.list.push_front(import);  \
  this->imports.num_##field##s++;  \
  /* Add to field */  \
  field##s.push_front(decl);  \
  return imports.list.front(); \
}

ImportDecl& WasmModule::add_import (ImportInfo &info, GlobalInfo &global) {
  IMPORT_INST (KIND_GLOBAL);

  GlobalDecl gdecl = {
    .type = global.type,
    .is_mutable = global.is_mutable
  };
  IMPORT_ADD(global, gdecl);
}

ImportDecl& WasmModule::add_import (ImportInfo &info, TableDecl &table) {
  IMPORT_INST (KIND_TABLE);
  IMPORT_ADD (table, table);
}

ImportDecl& WasmModule::add_import (ImportInfo &info, MemoryDecl &mem) {
  IMPORT_INST (KIND_MEMORY);
  IMPORT_ADD (mem, mem);
}

ImportDecl& WasmModule::add_import (ImportInfo &info, SigDecl &sig) {
  IMPORT_INST (KIND_FUNC);

  auto &sigs = this->sigs;
  auto &funcs = this->funcs;
  sigs.push_back(sig);
  FuncDecl fdecl = {
    .sig = &sigs.back()
  };
  IMPORT_ADD (func, fdecl);
}
