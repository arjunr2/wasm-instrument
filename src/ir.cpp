#include "ir.h"
#include "inst_internal.h"

const char* wasm_type_string(wasm_type_t type) {
  switch (type) {
    case WASM_TYPE_I32:       return "i32"; break;
    case WASM_TYPE_I64:       return "i64"; break;
    case WASM_TYPE_F32:       return "f32"; break;
    case WASM_TYPE_F64:       return "f64"; break;
    case WASM_TYPE_V128:      return "v128"; break;
    case WASM_TYPE_EXTERNREF: return "externref"; break;
    case WASM_TYPE_FUNCREF:   return "funcref"; break;
    default:  
      return "unknown";
  }
}

const char* wasm_kind_string(wasm_kind_t kind) {
  switch (kind) {
    case KIND_FUNC:     return "func"; break;
    case KIND_TABLE:    return "table"; break;
    case KIND_MEMORY:   return "memory"; break;
    case KIND_GLOBAL:   return "global"; break;
    default:  
      return "unknown";
  }
}

// Returns the string name of a section code.
const char* wasm_section_name(byte code) {
  switch (code) {
  case WASM_SECT_TYPE: return "type";
  case WASM_SECT_IMPORT: return "import";
  case WASM_SECT_FUNCTION: return "function";
  case WASM_SECT_TABLE: return "table";
  case WASM_SECT_MEMORY: return "memory";
  case WASM_SECT_GLOBAL: return "global";
  case WASM_SECT_EXPORT: return "export";
  case WASM_SECT_START: return "start";
  case WASM_SECT_ELEMENT: return "element";
  case WASM_SECT_CODE: return "code";
  case WASM_SECT_DATA: return "data";
  case WASM_SECT_DATACOUNT: return "datacount";
  case WASM_SECT_CUSTOM: return "custom";
  default:
    return "unknown";
  }
}


#define REASSIGN(val, ispace) ({ \
  this->get##ispace(mod.get##ispace##Idx(val));   \
})
WasmModule::WasmModule (const WasmModule &mod) {
  this->magic = mod.magic;
  this->version = mod.version;

  //
  this->customs = mod.customs;
  this->sigs = mod.sigs;

  this->imports = mod.imports;

  this->funcs = mod.funcs;
  this->tables = mod.tables;
  this->mems = mod.mems;
  this->globals = mod.globals;

  this->exports = mod.exports;
  
  this->elems = mod.elems;

  this->datas = mod.datas;

  this->has_start = mod.has_start;
  this->start_fn = REASSIGN(mod.start_fn, Func);

  this->has_datacount = mod.has_datacount;
  this->num_datas_datacount = mod.num_datas_datacount;

  // Patching
  DescriptorPatch<ImportDecl> (this->imports.list, mod);
  DescriptorPatch<ExportDecl> (this->exports, mod);
  FunctionPatch (mod);
  for (auto &elem : this->elems) {
    for (auto &fn_ptr : elem.func_indices) {
      fn_ptr = REASSIGN(fn_ptr, Func);
    }
  }

  CustomPatch (mod);
}


/* Descriptor patching for copy constructor */
template<typename T>
void WasmModule::DescriptorPatch (std::list<T> &list, const WasmModule &mod) {
  for (auto &v : list) {
    switch (v.kind) {
      case KIND_FUNC: v.desc.func = REASSIGN(v.desc.func, Func); break;
      case KIND_TABLE: v.desc.table = REASSIGN(v.desc.table, Table); break;
      case KIND_MEMORY: v.desc.mem = REASSIGN(v.desc.mem, Memory); break;
      case KIND_GLOBAL: v.desc.global = REASSIGN(v.desc.global, Global); break;
      default:
        ERR("Import kind: %u\n", v.kind);
        throw std::runtime_error("Invalid import kind");
    }
  }
}


#define PCS(cs, clsname, ...)  \
  case cs: {  \
    auto ispec = static_pointer_cast<clsname>(instptr); \
    instptr.reset(new clsname(instptr->getOpcode() __VA_OPT__(,) __VA_ARGS__ )); break;  \
  }
void WasmModule::FunctionPatch (const WasmModule &mod) {
  for (auto &func : this->funcs) {
    func.sig = REASSIGN(func.sig, Sig);
    for (auto institr = func.instructions.begin(); institr != func.instructions.end(); ++institr) {
      InstBasePtr &instptr = *institr;
      switch (instptr->getImmType()) {
        PCS (IMM_NONE, ImmNoneInst);
        PCS (IMM_BLOCKT, ImmBlocktInst, ispec->getType());
        PCS (IMM_LABEL, ImmLabelInst, ispec->getLabel());
        PCS (IMM_LABELS, ImmLabelsInst, ispec->getLabels(), ispec->getDefLabel());
        PCS (IMM_FUNC, ImmFuncInst, REASSIGN(ispec->getFunc(), Func));
        PCS (IMM_SIG_TABLE, ImmSigTableInst, REASSIGN(ispec->getSig(), Sig), REASSIGN(ispec->getTable(), Table));
        PCS (IMM_LOCAL, ImmLocalInst, ispec->getLocal());
        PCS (IMM_GLOBAL, ImmGlobalInst, REASSIGN(ispec->getGlobal(), Global));
        PCS (IMM_TABLE, ImmTableInst, REASSIGN(ispec->getTable(), Table));
        PCS (IMM_MEMARG, ImmMemargInst, ispec->getAlign(), ispec->getOffset());
        PCS (IMM_I32, ImmI32Inst, ispec->getValue());
        PCS (IMM_F64, ImmF64Inst, ispec->getValue());
        PCS (IMM_MEMORY, ImmMemoryInst, REASSIGN(ispec->getMemory(), Memory));
        PCS (IMM_TAG, ImmTagInst);
        PCS (IMM_I64, ImmI64Inst, ispec->getValue());
        PCS (IMM_F32, ImmF32Inst, ispec->getValue());
        PCS (IMM_REFNULLT, ImmRefnulltInst, ispec->getType());
        PCS (IMM_VALTS, ImmValtsInst, ispec->getTypes());
        // Extension Immediates
        PCS (IMM_DATA_MEMORY, ImmDataMemoryInst, REASSIGN(ispec->getData(), Data), REASSIGN(ispec->getMemory(), Memory));
        PCS (IMM_DATA, ImmDataInst, REASSIGN(ispec->getData(), Data));
        PCS (IMM_MEMORY_CP, ImmMemoryCpInst, REASSIGN(ispec->getDestMemory(), Memory), REASSIGN(ispec->getSrcMemory(), Memory));
      }
    }
  }
}


void WasmModule::CustomPatch (const WasmModule &mod) {
  for (auto &custom : this->customs) {
    if (custom.name == "name") {
      DebugNameDecl &debug = custom.debug;
      if (!debug.func_assoc.empty()) {
        for (auto &name_asc : debug.func_assoc) {
          name_asc.func = REASSIGN(name_asc.func, Func);
        }
      }
      this->fn_names_debug = &debug.func_assoc;
    }
  }
}
