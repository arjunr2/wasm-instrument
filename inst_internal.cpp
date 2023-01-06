#include <cstring>
#include "inst_internal.h"

///* InstBase encode error for unimplemented classes */
void InstBase::encode_imm (bytedeque &bdeq) const {
  ERR("Opcode: %u\n", opcode);
  throw std::runtime_error("Encode unimplemented for this opcode\n");
}

/* ImmNoneInst  */
ImmNoneInst::ImmNoneInst (WasmModule &module, byte opcode, buffer_t &buf)
                          : InstBase(opcode) { }

void ImmNoneInst::encode_imm (bytedeque &bdeq) const {}


/* ImmBlocktInst  */
ImmBlocktInst::ImmBlocktInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  int64_t value_type = RD_I64();
  if (value_type != -64) {
    ERR("Block type: %ld\n", value_type);
    throw std::runtime_error("Block type without 0x40 unsupported");
  }
  this->type = value_type;
}

void ImmBlocktInst::encode_imm (bytedeque &bdeq) const {}


/* ImmLabelInst  */
ImmLabelInst::ImmLabelInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();  
}

void ImmLabelInst::encode_imm (bytedeque &bdeq) const {}


/* ImmLabelsInst  */
ImmLabelsInst::ImmLabelsInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t num_labels = RD_U32();
  for (uint32_t i = 0; i < num_labels; i++) {
    this->idxs.push_back(RD_U32());
  }
  this->def_idx = RD_U32();
}

void ImmLabelsInst::encode_imm (bytedeque &bdeq) const {}


/* ImmFuncInst  */
ImmFuncInst::ImmFuncInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->func = module.getFunc (RD_U32());
}

void ImmFuncInst::encode_imm (bytedeque &bdeq) const {}


/* ImmSigTableInst  */
ImmSigTableInst::ImmSigTableInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->sig = module.getSig (RD_U32());
  this->func = module.getFunc (RD_U32());
}

void ImmSigTableInst::encode_imm (bytedeque &bdeq) const {}


/* ImmLocalInst  */
ImmLocalInst::ImmLocalInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();
}

void ImmLocalInst::encode_imm (bytedeque &bdeq) const {}


/* ImmGlobalInst  */
ImmGlobalInst::ImmGlobalInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->global = module.getGlobal (RD_U32());
}

void ImmGlobalInst::encode_imm (bytedeque &bdeq) const {}


/* ImmTableInst  */
ImmTableInst::ImmTableInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->table = module.getTable (RD_U32());
}

void ImmTableInst::encode_imm (bytedeque &bdeq) const {}


/* ImmMemargInst  */
ImmMemargInst::ImmMemargInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->align = RD_U32();
  this->offset = RD_U32();
}

void ImmMemargInst::encode_imm (bytedeque &bdeq) const {}


/* ImmI32Inst  */
ImmI32Inst::ImmI32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I32();
}

void ImmI32Inst::encode_imm (bytedeque &bdeq) const {}


/* ImmF64Inst  */
ImmF64Inst::ImmF64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint64_t rawbits = RD_U64_RAW();
  memcpy(&this->value, &rawbits, sizeof(double));
}

void ImmF64Inst::encode_imm (bytedeque &bdeq) const {}


/* ImmMemoryInst  */
ImmMemoryInst::ImmMemoryInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->mem = module.getMemory (RD_U32());
}


void ImmMemoryInst::encode_imm (bytedeque &bdeq) const {}

/* UNIMPLEMENTED */
/* ImmTagInst  */
ImmTagInst::ImmTagInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {}

void ImmTagInst::encode_imm (bytedeque &bdeq) const {}


/* ImmI64Inst  */
ImmI64Inst::ImmI64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I64();
}

void ImmI64Inst::encode_imm (bytedeque &bdeq) const {}


/* ImmF32Inst  */
ImmF32Inst::ImmF32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t rawbits = RD_U32_RAW();
  memcpy(&this->value, &rawbits, sizeof(float));
}

void ImmF32Inst::encode_imm (bytedeque &bdeq) const {}


/* ImmRefnulltInst  */
ImmRefnulltInst::ImmRefnulltInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  wasm_type_t type = (wasm_type_t) RD_BYTE();
  if (!isReftype(type)) {
    throw std::runtime_error("RefNull instruction must take a reftype\n");
  }
  this->type = (wasm_type_t) type;
}

void ImmRefnulltInst::encode_imm (bytedeque &bdeq) const {}


/* ImmValtsInst  */
ImmValtsInst::ImmValtsInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t num_vals = RD_U32();
  for (uint32_t i = 0; i < num_vals; i++) {
    wasm_type_t type = (wasm_type_t) RD_BYTE();
    this->types.push_back(type);
  }
}

void ImmValtsInst::encode_imm (bytedeque &bdeq) const {}

