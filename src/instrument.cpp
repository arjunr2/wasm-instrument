#include "ir.h"

#define FIND_OR_CREATE_SIG(sig) ({ \
  auto &sigs = this->sigs;  \
  /* Check for existing matching sig */ \
  SigDecl* sigptr = NULL; \
  for (auto &insig : sigs) {  \
    if (insig == sig) { \
      sigptr = &insig;  \
      break;  \
    } \
  } \
  /* Create new sig if not found */ \
  if (sigptr == NULL) { \
    sigs.push_back(sig);  \
    sigptr = &sigs.back();  \
  } \
  sigptr; \
})

/*** Instrumentation Functions ***/

#define ADD_FIELD(field) { \
  this->field##s.push_back(field);  \
  auto &ref = this->field##s.back();  \
  if (export_name) {  \
    add_export(export_name, ref); \
  } \
  return &ref; \
}

/* Add a global to a module */
GlobalDecl* WasmModule::add_global (GlobalDecl &global, const char* export_name) {
  ADD_FIELD (global);
}

TableDecl* WasmModule::add_table (TableDecl &table, const char* export_name) {
  ADD_FIELD (table);
}

MemoryDecl* WasmModule::add_memory (MemoryDecl &mem, const char* export_name) {
  ADD_FIELD (mem); 
}

FuncDecl* WasmModule::add_func (FuncDecl &func, const char* export_name) {
  SigDecl &sig = *(func.sig);
  SigDecl* sigptr = FIND_OR_CREATE_SIG(sig);
  func.sig = sigptr;
  ADD_FIELD (func);
}

#define IMPORT_INJ_KIND_GLOBAL(objref)
#define IMPORT_INJ_KIND_TABLE(objref)
#define IMPORT_INJ_KIND_MEMORY(objref)
/* Debug Info only for Func Kind if it has info */
#define IMPORT_INJ_KIND_FUNC(objref) {  \
  if (this->fn_names_debug) {  \
    DebugNameAssoc dname = { .func = &this->funcs.front(), .name = info.member_name };  \
    this->fn_names_debug->push_front (dname); \
  } \
}

#define IMPORT_ADD(kd, field, decl) { \
  auto &field##s = this->field##s;  \
  auto &imports = this->imports;  \
  /* Add to field */  \
  field##s.push_front(decl);  \
  /* Add to import list and count */  \
  ImportDecl import = { \
    .mod_name = info.mod_name,  \
    .member_name = info.member_name, \
    .kind = kd,  \
    .desc = { .field = &field##s.front() } \
  };  \
  this->imports.num_##field##s++;  \
  imports.list.push_front(import);  \
  IMPORT_INJ_##kd (field##s.front());  \
  return &imports.list.front(); \
}

/* Imports for all declarations */
ImportDecl* WasmModule::add_import (ImportInfo &info, GlobalInfo &global) {
  GlobalDecl gdecl = {
    .type = global.type,
    .is_mutable = global.is_mutable
  };
  IMPORT_ADD(KIND_GLOBAL, global, gdecl);
}

ImportDecl* WasmModule::add_import (ImportInfo &info, TableDecl &table) {
  IMPORT_ADD (KIND_TABLE, table, table);
}

ImportDecl* WasmModule::add_import (ImportInfo &info, MemoryDecl &mem) {
  IMPORT_ADD (KIND_MEMORY, mem, mem);
}


ImportDecl* WasmModule::add_import (ImportInfo &info, SigDecl &sig) {
  SigDecl* sigptr = FIND_OR_CREATE_SIG(sig);
  FuncDecl fdecl = { .sig = sigptr };
  IMPORT_ADD (KIND_FUNC, func, fdecl);
}



#define EXPORT_ADD(kd, field) { \
  /* Add to export list */  \
  auto &exports = this->exports;  \
  ExportDecl exp = { \
    .name = export_name,  \
    .kind = kd,  \
    .desc = { .field = &field } \
  };  \
  exports.push_back(exp); \
  return &exports.back(); \
}

/* Exports for all declarations */
ExportDecl* WasmModule::add_export (std::string export_name, GlobalDecl &global) {
  EXPORT_ADD (KIND_GLOBAL, global);
}

ExportDecl* WasmModule::add_export (std::string export_name, TableDecl &table) {
  EXPORT_ADD (KIND_TABLE, table);
}

ExportDecl* WasmModule::add_export (std::string export_name, MemoryDecl &mem) {
  EXPORT_ADD (KIND_MEMORY, mem);
}

ExportDecl* WasmModule::add_export (std::string export_name, FuncDecl &func) {
  EXPORT_ADD (KIND_FUNC, func);
}


/* Find methods */
ExportDecl* WasmModule::find_export (std::string export_name) {
  for (auto &exp : this->exports) {
    if (exp.name == export_name) { return &exp; }
  }
  return NULL;
}


/* Replace methods */
void WasmModule::replace_all_uses (GlobalDecl* old_inst, GlobalDecl* new_inst) {}

void WasmModule::replace_all_uses (TableDecl* old_inst, TableDecl* new_inst) {}

void WasmModule::replace_all_uses (MemoryDecl* old_inst, MemoryDecl* new_inst) {}

void WasmModule::replace_all_uses (FuncDecl* old_inst, FuncDecl* new_inst) {}
