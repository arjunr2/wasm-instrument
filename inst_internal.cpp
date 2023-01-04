#include "inst_internal.h"

ImmNoneInst::ImmNoneInst (byte opcode, buffer_t &buf) : InstBase(opcode) { }

ImmBlocktInst::ImmBlocktInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  byte value_type = RD_BYTE();
  if (value_type != 0x40) {
    throw std::runtime_error("Unimplemented block type without 0x40");
  }
}

ImmLabelInst::ImmLabelInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32();  
}


ImmLabelsInst::ImmLabelsInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  uint32_t num_labels = RD_U32();
  for (uint32_t i = 0; i < num_labels; i++) {
    RD_U32();
  }
  RD_U32();
}


ImmFuncInst::ImmFuncInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32();
}


ImmSigTableInst::ImmSigTableInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32();
  RD_U32();
}


ImmLocalInst::ImmLocalInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32();
}


ImmGlobalInst::ImmGlobalInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32();
}


ImmTableInst::ImmTableInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32();
}


ImmMemargInst::ImmMemargInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32();
  RD_U32();
}


ImmI32Inst::ImmI32Inst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_I32();
}


ImmF64Inst::ImmF64Inst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U64_RAW();
}


ImmMemoryInst::ImmMemoryInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  byte val = RD_BYTE();
  if (val) {
    throw std::runtime_error("Memory Immediate must be 0\n");
  }
}

/* Unimplemented */
ImmTagInst::ImmTagInst (byte opcode, buffer_t &buf) : InstBase(opcode) {

}


ImmI64Inst::ImmI64Inst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_I64();
}


ImmF32Inst::ImmF32Inst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_U32_RAW();
}


ImmRefnulltInst::ImmRefnulltInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  RD_BYTE();
}


ImmValtsInst::ImmValtsInst (byte opcode, buffer_t &buf) : InstBase(opcode) {
  uint32_t num_vals = RD_U32();
  for (uint32_t i = 0; i < num_vals; i++) {
    RD_BYTE();
  }
}

