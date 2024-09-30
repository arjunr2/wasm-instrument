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

SigDecl* WasmModule::add_sig (SigDecl &sig, bool force_dup) {
  if (force_dup) {
    this->sigs.push_back(sig);
    return &(this->sigs.back());
  } else {
    return FIND_OR_CREATE_SIG(sig);
  }
}


#define ADD_FIELD(field) ({ \
  this->field##s.push_back(field);  \
  auto &ref = this->field##s.back();  \
  if (export_name) {  \
    add_export(export_name, ref); \
  } \
  &ref; \
})

/* Add a global to a module */
GlobalDecl* WasmModule::add_global (GlobalDecl &global, const char* export_name) {
  return ADD_FIELD (global);
}

TableDecl* WasmModule::add_table (TableDecl &table, const char* export_name) {
  return ADD_FIELD (table);
}

MemoryDecl* WasmModule::add_memory (MemoryDecl &mem, const char* export_name) {
  return ADD_FIELD (mem);
}

FuncDecl* WasmModule::add_func (FuncDecl &func, const char* export_name, const char* debug_name) {
  func.sig = this->add_sig(*(func.sig), false);
  FuncDecl* ifn = ADD_FIELD (func);
  if (this->fn_names_debug) {
    DebugNameAssoc dname = { .func = ifn, .name = debug_name };
    this->fn_names_debug->push_back (dname);
  }
  return ifn;
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
  SigDecl* sigptr = this->add_sig(sig, false);
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

FuncDecl* WasmModule::find_func_from_debug_name (std::string debug_name) {
  if (auto debug_names_ptr = this->getFnDebugNames()) {
    auto &debug_names = *debug_names_ptr;
    for (auto &dname : debug_names) {
      if (dname.name == debug_name) {
        return dname.func;
      }
    }
  }
  return NULL;
}

#define IMPORT_FIND(kd, field) { \
  for (auto &import: this->imports.list) { \
    if (import.mod_name == mod_name && import.member_name == member_name && kd == import.kind)  \
      return import.desc.field; \
  } \
  return NULL;  \
}

GlobalDecl* WasmModule::find_import_global (std::string mod_name, std::string member_name) {
  IMPORT_FIND (KIND_GLOBAL, global);
}

TableDecl* WasmModule::find_import_table (std::string mod_name, std::string member_name) {
  IMPORT_FIND (KIND_TABLE, table);
}

MemoryDecl* WasmModule::find_import_memory (std::string mod_name, std::string member_name) {
  IMPORT_FIND (KIND_MEMORY, mem);
}

FuncDecl* WasmModule::find_import_func (std::string mod_name, std::string member_name) {
  IMPORT_FIND (KIND_FUNC, func);
}


/* Getter methods */
ImportDecl* WasmModule::get_import_name_from_func (FuncDecl *func) {
  if (!this->isImport(func)) {
    return NULL;
  }
  uint32_t func_ct = 0;
  for (auto &imp: this->imports.list) {
    if (func_ct >= this->imports.num_funcs) {
      // All funcs have been seen already
      return NULL;
    }
    if (imp.kind == KIND_FUNC) {
      func_ct++;
      if (imp.desc.func == func) {
        return &imp;
      }
    }
  }
  return NULL;
}


/* Removal methods */
bool WasmModule::remove_func_core (uint32_t idx, FuncDecl *func) {
  auto &funcs = this->Funcs();
  auto &imports = this->imports;
  if (idx >= funcs.size()) {
    return false;
  }

  auto func_itr = std::next(funcs.begin(), idx);
  // Remove reference in import section (if applicable)
  if (idx < imports.num_funcs) {
    auto import_itr = imports.list.begin();
    for (; import_itr != imports.list.end(); ++import_itr) {
      auto &imp = *import_itr;
      if ((imp.kind == KIND_FUNC) && (imp.desc.func == &(*func_itr))) {
        imports.list.erase(import_itr);
        imports.num_funcs--;
        break;
      }
    }
    // If no match, return false
    if (import_itr == imports.list.end()) {
      return false;
    }
  }
  // Remove reference in custom name section
  if (auto debug_names_ptr = this->getFnDebugNames()) {
    auto &debug_names = *debug_names_ptr;
    for (auto dname_itr = debug_names.begin(); dname_itr != debug_names.end(); ++dname_itr) {
      if (dname_itr->func == func) {
        debug_names.erase(dname_itr);
        break;
      }
    }
  }
  // Remove function
  funcs.erase(func_itr);
  return true;
}

bool WasmModule::remove_func (uint32_t idx) {
  return this->remove_func_core(idx, this->getFunc(idx));
}

bool WasmModule::remove_func (FuncDecl *func) {
  return this->remove_func_core(this->getFuncIdx(func), func);
}

/* Replace methods */
void WasmModule::replace_all_uses (GlobalDecl* old_inst, GlobalDecl* new_inst) {}

void WasmModule::replace_all_uses (TableDecl* old_inst, TableDecl* new_inst) {}

void WasmModule::replace_all_uses (MemoryDecl* old_inst, MemoryDecl* new_inst) {}

void WasmModule::replace_all_uses (FuncDecl* old_inst, FuncDecl* new_inst) {}
