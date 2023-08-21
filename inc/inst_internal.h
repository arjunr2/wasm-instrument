#pragma once
#include "common.h"
#include "wasmdefs.h"
#include "ir.h"

class BasicBlock {
  std::list<InstBasePtr>::iterator start;
  std::list<InstBasePtr>::iterator end;
  std::list<BasicBlock*> succs;
  std::list<BasicBlock*> preds;

  public:
  BasicBlock(std::list<InstBasePtr>::iterator start, 
              std::list<InstBasePtr>::iterator end): start(start), end(end) { }
  void add_successor(BasicBlock* succ) { this->succs.push_back(succ); }
  void add_predecessor(BasicBlock* pred) { this->preds.push_back(pred); }
};


/* All the following instructions should never be instantiated explicitly, only by parser */
class InstBase {
  private:
    uint16_t opcode;
    opcode_imm_type imm_type;
  public:
    InstBase (uint16_t opcode) : opcode(opcode) {
      this->imm_type = opcode_table[opcode].imm_type;
    }
    
    inline uint16_t getOpcode() { return opcode; }
    inline bool is(uint16_t opc) { return (this->opcode == opc); } 
    inline opcode_imm_type getImmType() { return imm_type; }
    inline bool isImmType(opcode_imm_type oit) { return (this->imm_type == oit); }

    virtual void encode_imm (WasmModule &module, bytedeque &bdeq) const;
};


/* Immediate types intermediate class 
* Decode immediate is performed within (module,byte,buf) constructor context
* Encode immediate is a virtual method
* Constructor to directly pass immediates as well
* */
class ImmNoneInst : public InstBase {
  public:
    ImmNoneInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmNoneInst (uint16_t opcode) : 
      InstBase(opcode) { };

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmBlocktInst: public InstBase {
  int64_t type;
  public:
    ImmBlocktInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmBlocktInst (uint16_t opcode, int64_t type) : 
      InstBase(opcode), type(type) { }

    inline int64_t getType() { return type; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelInst: public InstBase {
  uint32_t idx;
  public:
    ImmLabelInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmLabelInst (uint16_t opcode, uint32_t idx) : 
      InstBase(opcode), idx(idx) { }

    inline uint32_t getLabel() { return idx; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelsInst: public InstBase {
  std::list<uint32_t> idxs;
  uint32_t def_idx;
  public:
    ImmLabelsInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmLabelsInst (uint16_t opcode, std::list<uint32_t> idxs, uint32_t def_idx) : 
      InstBase(opcode), idxs(idxs), def_idx(def_idx) { }

    inline std::list<uint32_t>& getLabels() { return idxs; }
    inline uint32_t getDefLabel()           { return def_idx; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmFuncInst: public InstBase {
  FuncDecl* func;
  public:
    ImmFuncInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmFuncInst (uint16_t opcode, FuncDecl* func) :
      InstBase(opcode), func(func) { }

    inline FuncDecl* getFunc() { return func; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmSigTableInst: public InstBase {
  SigDecl* sig;
  TableDecl* table;
  public:
    ImmSigTableInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmSigTableInst (uint16_t opcode, SigDecl *sig, TableDecl *table) : 
      InstBase(opcode), sig(sig), table(table) { }

    inline SigDecl* getSig()      { return sig; }
    inline TableDecl* getTable()  { return table; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLocalInst: public InstBase {
  uint32_t idx;
  public:
    ImmLocalInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmLocalInst (uint16_t opcode, uint32_t idx) : 
      InstBase(opcode), idx(idx) { }

    inline uint32_t getLocal() { return idx; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmGlobalInst: public InstBase {
  GlobalDecl* global;
  public:
    ImmGlobalInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmGlobalInst (uint16_t opcode, GlobalDecl* global) : 
      InstBase(opcode), global(global) { }

    inline GlobalDecl* getGlobal() { return global; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmTableInst: public InstBase {
  TableDecl* table;
  public:
    ImmTableInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmTableInst (uint16_t opcode, TableDecl* table) : 
      InstBase(opcode), table(table) { }

    inline TableDecl* getTable() { return table; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemargInst: public InstBase {
  uint32_t align;
  uint32_t offset;
  public:
    ImmMemargInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmMemargInst (uint16_t opcode, uint32_t align, uint32_t offset) : 
      InstBase(opcode), align(align), offset(offset) { }

    inline uint32_t getOffset()  { return offset; }
    inline uint32_t getAlign()   { return align; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI32Inst: public InstBase {
  int32_t value;
  public:
    ImmI32Inst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmI32Inst (uint16_t opcode, int32_t value) : 
      InstBase(opcode), value(value) { }

    inline int32_t getValue() { return value; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF64Inst: public InstBase {
  double value;
  public:
    ImmF64Inst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmF64Inst (uint16_t opcode, double value) : 
      InstBase(opcode), value(value) { }

    inline double getValue() { return value; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemoryInst: public InstBase {
  MemoryDecl* mem;
  public:
    ImmMemoryInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmMemoryInst (uint16_t opcode, MemoryDecl* mem) : 
      InstBase(opcode), mem(mem) { }

    inline MemoryDecl* getMemory() { return mem; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


/* Unimplemented */
class ImmTagInst: public InstBase {

  public:
    ImmTagInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmTagInst (uint16_t opcode) :
      InstBase(opcode) { }
    //void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI64Inst: public InstBase {
  int64_t value;  
  public:
    ImmI64Inst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmI64Inst (uint16_t opcode, int64_t value) : 
      InstBase(opcode), value(value) { }

    inline int64_t getValue() { return value; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF32Inst: public InstBase {
  float value;
  public:
    ImmF32Inst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmF32Inst (uint16_t opcode, float value) : 
      InstBase(opcode), value(value) { }

    inline float getValue() { return value; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmRefnulltInst: public InstBase {
  wasm_type_t type;
  public:
    ImmRefnulltInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmRefnulltInst (uint16_t opcode, wasm_type_t type) : 
      InstBase(opcode), type(type) { }

    inline wasm_type_t getType() { return type; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmValtsInst: public InstBase {
  std::list<wasm_type_t> types;
  public:
    ImmValtsInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmValtsInst (uint16_t opcode, std::list<wasm_type_t> types) : 
      InstBase(opcode), types(types) { }

    inline std::list<wasm_type_t>& getTypes() { return types; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmDataMemoryInst: public InstBase {
  DataDecl* data;
  MemoryDecl* mem;
  public:
    ImmDataMemoryInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmDataMemoryInst (uint16_t opcode, DataDecl* data, MemoryDecl* mem) : 
      InstBase(opcode), data(data), mem(mem) { }

    inline DataDecl* getData()      { return data; }
    inline MemoryDecl* getMemory()  { return mem; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmDataInst: public InstBase {
  DataDecl* data;
  public:
    ImmDataInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmDataInst (uint16_t opcode, DataDecl* data) : 
      InstBase(opcode), data(data) { }

    inline DataDecl* getData()      { return data; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemorycpInst: public InstBase {
  MemoryDecl* dst;
  MemoryDecl* src;
  public:
    ImmMemorycpInst (WasmModule &module, uint16_t opcode, buffer_t &buf);
    ImmMemorycpInst (uint16_t opcode, MemoryDecl* dst, MemoryDecl* src) : 
      InstBase(opcode), dst(dst), src(src) { }

    inline MemoryDecl* getDestMemory()  { return dst; }
    inline MemoryDecl* getSrcMemory()  { return src; }

    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};
