#include "inst_internal.h"

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
  uint32_t fn_idx = RD_U32();
}


ImmSigTableInst::ImmSigTableInst (WasmModule &module, byte opcode, buffer_t &buf)
  : InstBase(opcode) {
  RD_U32();
  RD_U32();
}


ImmLocalInst::ImmLocalInst (WasmModule &module, byte opcode, buffer_t &buf)
  : InstBase(opcode) {
  RD_U32();
}


ImmGlobalInst::ImmGlobalInst (WasmModule &module, byte opcode, buffer_t &buf)
  : InstBase(opcode) {
  RD_U32();
}


ImmTableInst::ImmTableInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  RD_U32();
}


ImmMemargInst::ImmMemargInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  RD_U32();
  RD_U32();
}


ImmI32Inst::ImmI32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  RD_I32();
}


ImmF64Inst::ImmF64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  RD_U64_RAW();
}


ImmMemoryInst::ImmMemoryInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  byte val = RD_BYTE();
  if (val) {
    throw std::runtime_error("Memory Immediate must be 0\n");
  }
}

/* Unimplemented */
ImmTagInst::ImmTagInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {

}


ImmI64Inst::ImmI64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  RD_I64();
}


ImmF32Inst::ImmF32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  RD_U32_RAW();
}


ImmRefnulltInst::ImmRefnulltInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  RD_BYTE();
}


ImmValtsInst::ImmValtsInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t num_vals = RD_U32();
  for (uint32_t i = 0; i < num_vals; i++) {
    RD_BYTE();
  }
}

