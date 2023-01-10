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
* Constructor to directly pass immediates as well
* */
class ImmNoneInst : public InstBase {
  public:
    ImmNoneInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmNoneInst (byte opcode) : 
      InstBase(opcode) { };

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmBlocktInst: public InstBase {
  int64_t type;
  public:
    ImmBlocktInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmBlocktInst (byte opcode, int64_t type) : 
      InstBase(opcode), type(type) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelInst: public InstBase {
  uint32_t idx;
  public:
    ImmLabelInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmLabelInst (byte opcode, uint32_t idx) : 
      InstBase(opcode), idx(idx) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelsInst: public InstBase {
  std::list<uint32_t> idxs;
  uint32_t def_idx;
  public:
    ImmLabelsInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmLabelsInst (byte opcode, std::list<uint32_t> idxs, uint32_t def_idx) : 
      InstBase(opcode), idxs(idxs), def_idx(def_idx) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmFuncInst: public InstBase {
  FuncDecl* func;
  public:
    ImmFuncInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmFuncInst (byte opcode, FuncDecl* func) :
      InstBase(opcode), func(func) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmSigTableInst: public InstBase {
  SigDecl* sig;
  FuncDecl* func;
  public:
    ImmSigTableInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmSigTableInst (byte opcode, SigDecl *sig, FuncDecl *func) : 
      InstBase(opcode), sig(sig), func(func) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLocalInst: public InstBase {
  uint32_t idx;
  public:
    ImmLocalInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmLocalInst (byte opcode, uint32_t idx) : 
      InstBase(opcode), idx(idx) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmGlobalInst: public InstBase {
  GlobalDecl* global;
  public:
    ImmGlobalInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmGlobalInst (byte opcode, GlobalDecl* global) : 
      InstBase(opcode), global(global) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmTableInst: public InstBase {
  TableDecl* table;
  public:
    ImmTableInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmTableInst (byte opcode, TableDecl* table) : 
      InstBase(opcode), table(table) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemargInst: public InstBase {
  uint32_t align;
  uint32_t offset;
  public:
    ImmMemargInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmMemargInst (byte opcode, uint32_t align, uint32_t offset) : 
      InstBase(opcode), align(align), offset(offset) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI32Inst: public InstBase {
  uint32_t value;
  public:
    ImmI32Inst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmI32Inst (byte opcode, uint32_t value) : 
      InstBase(opcode), value(value) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF64Inst: public InstBase {
  double value;
  public:
    ImmF64Inst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmF64Inst (byte opcode, double value) : 
      InstBase(opcode), value(value) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemoryInst: public InstBase {
  MemoryDecl* mem;
  public:
    ImmMemoryInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmMemoryInst (byte opcode, MemoryDecl* mem) : 
      InstBase(opcode), mem(mem) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


/* Unimplemented */
class ImmTagInst: public InstBase {

  public:
    ImmTagInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmTagInst (byte opcode) :
      InstBase(opcode) { }
    //void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI64Inst: public InstBase {
  int64_t value;  
  public:
    ImmI64Inst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmI64Inst (byte opcode, double value) : 
      InstBase(opcode), value(value) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF32Inst: public InstBase {
  float value;
  public:
    ImmF32Inst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmF32Inst (byte opcode, float value) : 
      InstBase(opcode), value(value) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmRefnulltInst: public InstBase {
  wasm_type_t type;
  public:
    ImmRefnulltInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmRefnulltInst (byte opcode, wasm_type_t type) : 
      InstBase(opcode), type(type) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmValtsInst: public InstBase {
  std::list<wasm_type_t> types;
  public:
    ImmValtsInst (WasmModule &module, byte opcode, buffer_t &buf);
    ImmValtsInst (byte opcode, std::list<wasm_type_t> types) : 
      InstBase(opcode), types(types) { }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


