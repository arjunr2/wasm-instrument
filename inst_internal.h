#pragma once
#include "common.h"
#include "wasmdefs.h"
#include "ir.h"

/* All these should never be instantiated explicitly, only by parser */

class InstBase {
  private:
    byte opcode;
  public:
    InstBase (byte opcode) : opcode(opcode) {};
    
    inline byte getOpcode() { return opcode; }
    inline bool is(byte opc) { return (this->opcode == opc); } 

    virtual void encode_imm (WasmModule &module, bytedeque &bdeq) const;
};


/* Immediate types intermediate class 
* Decode immediate is performed within (module,byte,buf) constructor context
* Encode immediate is a virtual method
* */
class ImmNoneInst : public InstBase {
  public:
    ImmNoneInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmBlocktInst: public InstBase {
  int64_t type;
  public:
    ImmBlocktInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelInst: public InstBase {
  uint32_t idx;
  public:
    ImmLabelInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelsInst: public InstBase {
  std::list<uint32_t> idxs;
  uint32_t def_idx;
  public:
    ImmLabelsInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmFuncInst: public InstBase {
  FuncDecl* func;
  public:
    ImmFuncInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmSigTableInst: public InstBase {
  SigDecl* sig;
  FuncDecl* func;
  public:
    ImmSigTableInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLocalInst: public InstBase {
  uint32_t idx;
  public:
    ImmLocalInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmGlobalInst: public InstBase {
  GlobalDecl* global;
  public:
    ImmGlobalInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmTableInst: public InstBase {
  TableDecl* table;
  public:
    ImmTableInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemargInst: public InstBase {
  uint32_t align;
  uint32_t offset;
  public:
    ImmMemargInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI32Inst: public InstBase {
  uint32_t value;
  public:
    ImmI32Inst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF64Inst: public InstBase {
  double value;
  public:
    ImmF64Inst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemoryInst: public InstBase {
  MemoryDecl* mem;
  public:
    ImmMemoryInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


/* Unimplemented */
class ImmTagInst: public InstBase {

  public:
    ImmTagInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI64Inst: public InstBase {
  int64_t value;  
  public:
    ImmI64Inst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF32Inst: public InstBase {
  float value;
  public:
    ImmF32Inst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmRefnulltInst: public InstBase {
  wasm_type_t type;
  public:
    ImmRefnulltInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmValtsInst: public InstBase {
  std::list<wasm_type_t> types;
  public:
    ImmValtsInst (WasmModule &module, byte opcode, buffer_t &buf);
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


