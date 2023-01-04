#include "inst_internal.h"
#include <cstring>

ImmNoneInst::ImmNoneInst (WasmModule &module, byte opcode, buffer_t &buf)
                          : InstBase(opcode) { }

ImmBlocktInst::ImmBlocktInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  int64_t value_type = RD_I64();
  if (value_type != -64) {
    ERR("Block type: %ld\n", value_type);
    throw std::runtime_error("Block type without 0x40 unsupported");
  }
  this->type = value_type;
}

ImmLabelInst::ImmLabelInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();  
}


ImmLabelsInst::ImmLabelsInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t num_labels = RD_U32();
  for (uint32_t i = 0; i < num_labels; i++) {
    this->idxs.push_back(RD_U32());
  }
  this->def_idx = RD_U32();
}


ImmFuncInst::ImmFuncInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->func = module.getFunc (RD_U32());
}


ImmSigTableInst::ImmSigTableInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->sig = module.getSig (RD_U32());
  this->func = module.getFunc (RD_U32());
}


ImmLocalInst::ImmLocalInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();
}


ImmGlobalInst::ImmGlobalInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->global = module.getGlobal (RD_U32());
}


ImmTableInst::ImmTableInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->table = module.getTable (RD_U32());
}


ImmMemargInst::ImmMemargInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->align = RD_U32();
  this->offset = RD_U32();
}


ImmI32Inst::ImmI32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I32();
}


ImmF64Inst::ImmF64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint64_t rawbits = RD_U64_RAW();
  memcpy(&this->value, &rawbits, sizeof(double));
}


ImmMemoryInst::ImmMemoryInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->mem = module.getMemory (RD_U32());
}

/* Unimplemented */
ImmTagInst::ImmTagInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {}


ImmI64Inst::ImmI64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I64();
}


ImmF32Inst::ImmF32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t rawbits = RD_U32_RAW();
  memcpy(&this->value, &rawbits, sizeof(float));
}


ImmRefnulltInst::ImmRefnulltInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  wasm_type_t type = (wasm_type_t) RD_BYTE();
  if (!isReftype(type)) {
    throw std::runtime_error("RefNull instruction must take a reftype\n");
  }
  this->type = (wasm_type_t) type;
}


ImmValtsInst::ImmValtsInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t num_vals = RD_U32();
  for (uint32_t i = 0; i < num_vals; i++) {
    wasm_type_t type = (wasm_type_t) RD_BYTE();
    this->types.push_back(type);
  }
}

