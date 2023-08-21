#include <cstring>
#include "inst_internal.h"

/* InstBase encode error for unimplemented classes */
void InstBase::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  ERR("Opcode: %u\n", opcode);
  throw std::runtime_error("Encode unimplemented for this opcode\n");
}

/* ImmNoneInst  */
ImmNoneInst::ImmNoneInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
                          : InstBase(opcode) { }

void ImmNoneInst::encode_imm (WasmModule &module, bytedeque &bdeq) const { }


/* ImmBlocktInst  */
ImmBlocktInst::ImmBlocktInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  int64_t value_type = RD_I64();
  if (value_type >= 0) {
    ERR("Block type: %ld\n", value_type);
    throw std::runtime_error("Block type with type section index unsupported!");
  }
  this->type = value_type;
}

void ImmBlocktInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_I64 (this->type);
}


/* ImmLabelInst  */
ImmLabelInst::ImmLabelInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();  
}

void ImmLabelInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->idx);
}


/* ImmLabelsInst  */
ImmLabelsInst::ImmLabelsInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
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
ImmFuncInst::ImmFuncInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->func = module.getFunc (RD_U32());
}

void ImmFuncInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getFuncIdx(this->func);
  WR_U32 (idx);
}


/* ImmSigTableInst  */
ImmSigTableInst::ImmSigTableInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->sig = module.getSig (RD_U32());
  this->table = module.getTable (RD_U32());
}

void ImmSigTableInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t sig_idx = module.getSigIdx (this->sig);
  uint32_t table_idx = module.getTableIdx (this->table);
  WR_U32 (sig_idx);
  WR_U32 (table_idx);
}


/* ImmLocalInst  */
ImmLocalInst::ImmLocalInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_U32();
}

void ImmLocalInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->idx);
}


/* ImmGlobalInst  */
ImmGlobalInst::ImmGlobalInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->global = module.getGlobal (RD_U32());
}

void ImmGlobalInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getGlobalIdx(this->global);
  WR_U32 (idx);
}


/* ImmTableInst  */
ImmTableInst::ImmTableInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->table = module.getTable (RD_U32());
}

void ImmTableInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getTableIdx(this->table);
  WR_U32 (idx);
}


/* ImmMemargInst  */
ImmMemargInst::ImmMemargInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->align = RD_U32();
  this->offset = RD_U32();
}

void ImmMemargInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->align);
  WR_U32 (this->offset);
}


/* ImmI32Inst  */
ImmI32Inst::ImmI32Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I32();
}

void ImmI32Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_I32 (this->value);
}


/* ImmF64Inst  */
ImmF64Inst::ImmF64Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
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
ImmMemoryInst::ImmMemoryInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->mem = module.getMemory (RD_U32());
}


void ImmMemoryInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t idx = module.getMemoryIdx(this->mem);
  WR_U32 (idx);
}


/* ImmTagInst: UNIMPLEMENTED  */
ImmTagInst::ImmTagInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) { 
  throw std::runtime_error ("Unimplemeted TAG opcode"); 
}

//void ImmTagInst::encode_imm (WasmModule &module, bytedeque &bdeq) const { } 


/* ImmI64Inst  */
ImmI64Inst::ImmI64Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value = RD_I64();
}

void ImmI64Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_I64 (this->value);
}


/* ImmF32Inst  */
ImmF32Inst::ImmF32Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
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
ImmRefnulltInst::ImmRefnulltInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
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
ImmValtsInst::ImmValtsInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
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


/* ImmDataMemoryInst  */
ImmDataMemoryInst::ImmDataMemoryInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->data = module.getData (RD_U32());
  this->mem = module.getMemory (RD_U32());
}

void ImmDataMemoryInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t data_idx = module.getDataIdx(this->data);
  WR_U32 (data_idx);
  uint32_t mem_idx = module.getMemoryIdx(this->mem);
  WR_U32 (mem_idx);
}


/* ImmDataInst  */
ImmDataInst::ImmDataInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->data = module.getData (RD_U32());
}

void ImmDataInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t data_idx = module.getDataIdx(this->data);
  WR_U32 (data_idx);
}


/* ImmMemorycpInst  */
ImmMemorycpInst::ImmMemorycpInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->dst = module.getMemory (RD_U32());
  this->src = module.getMemory (RD_U32());
}

void ImmMemorycpInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t dst_idx = module.getMemoryIdx(this->dst);
  WR_U32 (dst_idx);
  uint32_t src_idx = module.getMemoryIdx(this->src);
  WR_U32 (src_idx);
}





/* ImmDataTableInst  */
ImmDataTableInst::ImmDataTableInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->data = module.getData (RD_U32());
  this->table = module.getTable (RD_U32());
}

void ImmDataTableInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t data_idx = module.getDataIdx(this->data);
  WR_U32 (data_idx);
  uint32_t table_idx = module.getTableIdx(this->table);
  WR_U32 (table_idx);
}


/* ImmTablecpInst  */
ImmTablecpInst::ImmTablecpInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->dst = module.getTable (RD_U32());
  this->src = module.getTable (RD_U32());
}

void ImmTablecpInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  uint32_t dst_idx = module.getTableIdx(this->dst);
  WR_U32 (dst_idx);
  uint32_t src_idx = module.getTableIdx(this->src);
  WR_U32 (src_idx);
}


/* ImmV128Inst  */
ImmV128Inst::ImmV128Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->value.v[0] = RD_U64_RAW();
  this->value.v[1] = RD_U64_RAW();
}

void ImmV128Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U64_RAW (this->value.v[0]);
  WR_U64_RAW (this->value.v[1]);
}


/* ImmLaneidxInst  */
ImmLaneidxInst::ImmLaneidxInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->idx = RD_BYTE();
}

void ImmLaneidxInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_BYTE (this->idx);
}


/* ImmLaneidx16Inst  */
ImmLaneidx16Inst::ImmLaneidx16Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  for (int i = 0; i < 16; i++) {
    this->idxs.l[i] = RD_BYTE();
  }
}

void ImmLaneidx16Inst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  for (int i = 0; i < 16; i++) {
    WR_BYTE (this->idxs.l[i]);
  }
}


/* ImmMemargLaneidxInst  */
ImmMemargLaneidxInst::ImmMemargLaneidxInst (WasmModule &module, Opcode_t opcode, buffer_t &buf)
    : InstBase(opcode) {
  this->align = RD_U32();
  this->offset = RD_U32();
  this->laneidx = RD_BYTE();
}

void ImmMemargLaneidxInst::encode_imm (WasmModule &module, bytedeque &bdeq) const {
  WR_U32 (this->align);
  WR_U32 (this->offset);
  WR_BYTE (this->laneidx);
}
