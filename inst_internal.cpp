#include <cstring>
#include "inst_internal.h"

/* InstBase encode error for unimplemented classes */
void InstBase::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  ERR("Opcode: %u\n", opcode);
  throw std::runtime_error("Encode unimplemented for this opcode\n");
}

/* ImmNoneInst  */
ImmNoneInst::ImmNoneInst (WasmModule &module, byte opcode, buffer_t &buf)
                          : InstBase(opcode) { }

void ImmNoneInst::encode_imm (WasmModule &module, bytedeque &bdeq) const { }


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

void ImmBlocktInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_I64 (this->type);
}


/* ImmLabelInst  */
ImmLabelInst::ImmLabelInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();  
}

void ImmLabelInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->idx);
}


/* ImmLabelsInst  */
ImmLabelsInst::ImmLabelsInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t num_labels = RD_U32();
  for (uint32_t i = 0; i < num_labels; i++) {
    this->idxs.push_back(RD_U32());
  }
  this->def_idx = RD_U32();
}

void ImmLabelsInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->idxs.size());
  for (auto &idx : this->idxs) {
    WR_U32 (idx);
  }
  WR_U32 (this->def_idx);
}


/* ImmFuncInst  */
ImmFuncInst::ImmFuncInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->func = module.getFunc (RD_U32());
}

void ImmFuncInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getFuncIdx(this->func);
  WR_U32 (idx);
}


/* ImmSigTableInst  */
ImmSigTableInst::ImmSigTableInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->sig = module.getSig (RD_U32());
  this->func = module.getFunc (RD_U32());
}

void ImmSigTableInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t sig_idx = module.getSigIdx (this->sig);
  uint32_t func_idx = module.getFuncIdx (this->func);
  WR_U32 (sig_idx);
  WR_U32 (func_idx);
}


/* ImmLocalInst  */
ImmLocalInst::ImmLocalInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();
}

void ImmLocalInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->idx);
}


/* ImmGlobalInst  */
ImmGlobalInst::ImmGlobalInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->global = module.getGlobal (RD_U32());
}

void ImmGlobalInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getGlobalIdx(this->global);
}


/* ImmTableInst  */
ImmTableInst::ImmTableInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->table = module.getTable (RD_U32());
}

void ImmTableInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getTableIdx(this->table);
}


/* ImmMemargInst  */
ImmMemargInst::ImmMemargInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->align = RD_U32();
  this->offset = RD_U32();
}

void ImmMemargInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->align);
  WR_U32 (this->offset);
}


/* ImmI32Inst  */
ImmI32Inst::ImmI32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I32();
}

void ImmI32Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_I32 (this->value);
}


/* ImmF64Inst  */
ImmF64Inst::ImmF64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint64_t rawbits = RD_U64_RAW();
  memcpy(&this->value, &rawbits, sizeof(double));
}

void ImmF64Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint64_t rawbits;
  memcpy(&rawbits, &this->value, sizeof(uint64_t));
  WR_U64_RAW (rawbits);
}


/* ImmMemoryInst  */
ImmMemoryInst::ImmMemoryInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->mem = module.getMemory (RD_U32());
}


void ImmMemoryInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getMemoryIdx(this->mem);
  WR_U32 (idx);
}


/* UNIMPLEMENTED */
/* ImmTagInst  */
ImmTagInst::ImmTagInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {}

void ImmTagInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {}


/* ImmI64Inst  */
ImmI64Inst::ImmI64Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I64();
}

void ImmI64Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_I64 (this->value);
}


/* ImmF32Inst  */
ImmF32Inst::ImmF32Inst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t rawbits = RD_U32_RAW();
  memcpy(&this->value, &rawbits, sizeof(float));
}

void ImmF32Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t rawbits;
  memcpy (&rawbits, &this->value, sizeof(float));
  WR_U32_RAW (rawbits);
}


/* ImmRefnulltInst  */
ImmRefnulltInst::ImmRefnulltInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  wasm_type_t type = (wasm_type_t) RD_BYTE();
  if (!isReftype(type)) {
    throw std::runtime_error("RefNull instruction must take a reftype\n");
  }
  this->type = (wasm_type_t) type;
}

void ImmRefnulltInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_BYTE (this->type);
}


/* ImmValtsInst  */
ImmValtsInst::ImmValtsInst (WasmModule &module, byte opcode, buffer_t &buf)
    : InstBase(opcode) {
  uint32_t num_vals = RD_U32();
  for (uint32_t i = 0; i < num_vals; i++) {
    wasm_type_t type = (wasm_type_t) RD_BYTE();
    this->types.push_back(type);
  }
}

void ImmValtsInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->types.size());
  for (auto &type : this->types) {
    WR_BYTE (type);
  }
}

