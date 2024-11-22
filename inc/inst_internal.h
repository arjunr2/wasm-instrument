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
    Opcode_t opcode;
    opcode_imm_type imm_type;
    uint32_t orig_pc;
  public:
    InstBase (Opcode_t opcode) : opcode(opcode) {
      this->imm_type = opcode_table[opcode].imm_type;
      // We can't easily set the pc when we create the instruction since we need to 
      // backtrack the variable length opcode.
      // Instead set this after creation with setOrigPc
      this->orig_pc = -1;
    }

    inline Opcode_t getOpcode() { return opcode; }
    inline uint32_t getOrigPc() { return orig_pc; }
    inline void setOrigPc(uint32_t pc) { orig_pc = pc; }
    inline bool is(Opcode_t opc) { return (this->opcode == opc); } 
    inline opcode_imm_type getImmType() { return imm_type; }
    inline bool isImmType(opcode_imm_type oit) { return (this->imm_type == oit); }

    //virtual void decode_imm (WasmModule &module , buffer_t &buf) const = 0;
    virtual void encode_imm (WasmModule &module, bytedeque &bdeq) const = 0;
};


/* Immediate types intermediate class 
* Decode immediate is performed within (module,opcode,buf) constructor context
* Encode immediate is a virtual method
* Constructor to directly pass immediates as well
*/
class ImmNoneInst : public InstBase {
  public:
    ImmNoneInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmNoneInst (Opcode_t opcode) : 
      InstBase(opcode) { };

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmBlocktInst: public InstBase {
  int64_t type;
  public:
    ImmBlocktInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmBlocktInst (Opcode_t opcode, int64_t type) : 
      InstBase(opcode), type(type) { }

    inline int64_t getType() { return type; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelInst: public InstBase {
  uint32_t idx;
  public:
    ImmLabelInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmLabelInst (Opcode_t opcode, uint32_t idx) : 
      InstBase(opcode), idx(idx) { }

    inline uint32_t getLabel() { return idx; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLabelsInst: public InstBase {
  std::list<uint32_t> idxs;
  uint32_t def_idx;
  public:
    ImmLabelsInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmLabelsInst (Opcode_t opcode, std::list<uint32_t> idxs, uint32_t def_idx) : 
      InstBase(opcode), idxs(idxs), def_idx(def_idx) { }

    inline std::list<uint32_t>& getLabels() { return idxs; }
    inline uint32_t getDefLabel()           { return def_idx; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmFuncInst: public InstBase {
  FuncDecl* func;
  public:
    ImmFuncInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmFuncInst (Opcode_t opcode, FuncDecl* func) :
      InstBase(opcode), func(func) { }

    inline FuncDecl* getFunc() { return func; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmSigTableInst: public InstBase {
  SigDecl* sig;
  TableDecl* table;
  public:
    ImmSigTableInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmSigTableInst (Opcode_t opcode, SigDecl *sig, TableDecl *table) : 
      InstBase(opcode), sig(sig), table(table) { }

    inline SigDecl* getSig()      { return sig; }
    inline TableDecl* getTable()  { return table; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLocalInst: public InstBase {
  uint32_t idx;
  public:
    ImmLocalInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmLocalInst (Opcode_t opcode, uint32_t idx) : 
      InstBase(opcode), idx(idx) { }

    inline uint32_t getLocal() { return idx; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmGlobalInst: public InstBase {
  GlobalDecl* global;
  public:
    ImmGlobalInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmGlobalInst (Opcode_t opcode, GlobalDecl* global) : 
      InstBase(opcode), global(global) { }

    inline GlobalDecl* getGlobal() { return global; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmTableInst: public InstBase {
  TableDecl* table;
  public:
    ImmTableInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmTableInst (Opcode_t opcode, TableDecl* table) : 
      InstBase(opcode), table(table) { }

    inline TableDecl* getTable() { return table; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemargInst: public InstBase {
  uint32_t align;
  uint32_t offset;
  MemoryDecl *mem;
  public:
    ImmMemargInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmMemargInst (Opcode_t opcode, uint32_t align, uint32_t offset, MemoryDecl *mem) : 
      InstBase(opcode), align(align), offset(offset), mem(mem) { }

    inline uint32_t getOffset()  { return offset; }
    inline uint32_t getAlign()   { return align; }
    inline MemoryDecl* getMemory() { return mem; }

    inline void setMemory(MemoryDecl *mem) { this->mem = mem; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI32Inst: public InstBase {
  int32_t value;
  public:
    ImmI32Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmI32Inst (Opcode_t opcode, int32_t value) : 
      InstBase(opcode), value(value) { }

    inline int32_t getValue() { return value; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF64Inst: public InstBase {
  double value;
  public:
    ImmF64Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmF64Inst (Opcode_t opcode, double value) : 
      InstBase(opcode), value(value) { }

    inline double getValue() { return value; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemoryInst: public InstBase {
  MemoryDecl* mem;
  public:
    ImmMemoryInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmMemoryInst (Opcode_t opcode, MemoryDecl* mem) : 
      InstBase(opcode), mem(mem) { }

    inline MemoryDecl* getMemory() { return mem; }

    inline void setMemory(MemoryDecl *mem) { this->mem = mem; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


/* Unimplemented */
class ImmTagInst: public InstBase {

  public:
    ImmTagInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmTagInst (Opcode_t opcode) :
      InstBase(opcode) { }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmI64Inst: public InstBase {
  int64_t value;  
  public:
    ImmI64Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmI64Inst (Opcode_t opcode, int64_t value) : 
      InstBase(opcode), value(value) { }

    inline int64_t getValue() { return value; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmF32Inst: public InstBase {
  float value;
  public:
    ImmF32Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmF32Inst (Opcode_t opcode, float value) : 
      InstBase(opcode), value(value) { }

    inline float getValue() { return value; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmRefnulltInst: public InstBase {
  wasm_type_t type;
  public:
    ImmRefnulltInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmRefnulltInst (Opcode_t opcode, wasm_type_t type) : 
      InstBase(opcode), type(type) { }

    inline wasm_type_t getType() { return type; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmValtsInst: public InstBase {
  std::list<wasm_type_t> types;
  public:
    ImmValtsInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmValtsInst (Opcode_t opcode, std::list<wasm_type_t> types) : 
      InstBase(opcode), types(types) { }

    inline std::list<wasm_type_t>& getTypes() { return types; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};

// Extensions

class ImmDataMemoryInst: public InstBase {
  DataDecl* data;
  MemoryDecl* mem;
  public:
    ImmDataMemoryInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmDataMemoryInst (Opcode_t opcode, DataDecl* data, MemoryDecl* mem) : 
      InstBase(opcode), data(data), mem(mem) { }

    inline DataDecl* getData()      { return data; }
    inline MemoryDecl* getMemory()  { return mem; }

    inline void setMemory(MemoryDecl *mem) { this->mem = mem; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmDataInst: public InstBase {
  DataDecl* data;
  public:
    ImmDataInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmDataInst (Opcode_t opcode, DataDecl* data) : 
      InstBase(opcode), data(data) { }

    inline DataDecl* getData()      { return data; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemorycpInst: public InstBase {
  MemoryDecl* dst;
  MemoryDecl* src;
  public:
    ImmMemorycpInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmMemorycpInst (Opcode_t opcode, MemoryDecl* dst, MemoryDecl* src) : 
      InstBase(opcode), dst(dst), src(src) { }

    inline MemoryDecl* getDstMemory()  { return dst; }
    inline MemoryDecl* getSrcMemory()  { return src; }

    inline void setDstMemory(MemoryDecl *mem) { this->dst = mem; }
    inline void setSrcMemory(MemoryDecl *mem) { this->src = mem; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmDataTableInst: public InstBase {
  DataDecl* data;
  TableDecl* table;
  public:
    ImmDataTableInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmDataTableInst (Opcode_t opcode, DataDecl* data, TableDecl* table) : 
      InstBase(opcode), data(data), table(table) { }

    inline DataDecl* getData()  { return data; }
    inline TableDecl* getTable()  { return table; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmTablecpInst: public InstBase {
  TableDecl* dst;
  TableDecl* src;
  public:
    ImmTablecpInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmTablecpInst (Opcode_t opcode, TableDecl* dst, TableDecl* src) : 
      InstBase(opcode), dst(dst), src(src) { }

    inline TableDecl* getDstTable()  { return dst; }
    inline TableDecl* getSrcTable()  { return src; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmV128Inst: public InstBase {
  V128_t value;
  public:
    ImmV128Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmV128Inst (Opcode_t opcode, V128_t value) : 
      InstBase(opcode), value(value) { }

    inline V128_t getValue() { return value; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLaneidxInst: public InstBase {
  Laneidx_t idx;
  public:
    ImmLaneidxInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmLaneidxInst (Opcode_t opcode, Laneidx_t idx) : 
      InstBase(opcode), idx(idx) { }

    inline Laneidx_t getLaneidx() { return idx; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmLaneidx16Inst: public InstBase {
  Laneidx16_t idxs;
  public:
    ImmLaneidx16Inst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmLaneidx16Inst (Opcode_t opcode, Laneidx16_t idxs) : 
      InstBase(opcode), idxs(idxs) { }

    inline Laneidx16_t getLaneidx16() { return idxs; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


class ImmMemargLaneidxInst: public InstBase {
  uint32_t align;
  uint32_t offset;
  Laneidx_t laneidx;
  MemoryDecl *mem;
  public:
    ImmMemargLaneidxInst (WasmModule &module, Opcode_t opcode, buffer_t &buf);
    ImmMemargLaneidxInst (Opcode_t opcode, uint32_t align, uint32_t offset, MemoryDecl *mem, Laneidx_t laneidx) : 
      InstBase(opcode), align(align), offset(offset), mem(mem), laneidx(laneidx) { }

    inline uint32_t getOffset()  { return offset; }
    inline uint32_t getAlign()   { return align; }
    inline MemoryDecl* getMemory() { return mem; }
    inline Laneidx_t getLaneidx() { return laneidx; }

    inline void setMemory(MemoryDecl *mem) { this->mem = mem; }

    //void decode_imm (WasmModule &module , buffer_t &buf) const override;
    void encode_imm (WasmModule &module, bytedeque &bdeq) const override;
};


namespace InstParser {
  #define ICS(cs, clsname) \
    case cs: instptr.reset(new clsname(module, opcode, buf));  \
        instptr->setOrigPc(pc); break;
  /* Reads next instructions from buf (advancing it) and 
    constructs the specified instruction type */
  static InstBasePtr next_inst_from_buf(WasmModule &module, buffer_t &buf) {
    InstBasePtr instptr;
    // First instruction starts at PC=1
    uint32_t pc = buf.ptr - buf.start + 1;
    Opcode_t opcode = RD_OPCODE();
    opcode_imm_type imm_type = opcode_table[opcode].imm_type;
    switch (imm_type) {
      ICS (IMM_NONE, ImmNoneInst);
      ICS (IMM_BLOCKT, ImmBlocktInst);
      ICS (IMM_LABEL, ImmLabelInst);
      ICS (IMM_LABELS, ImmLabelsInst);
      ICS (IMM_FUNC, ImmFuncInst);
      ICS (IMM_SIG_TABLE, ImmSigTableInst);
      ICS (IMM_LOCAL, ImmLocalInst);
      ICS (IMM_GLOBAL, ImmGlobalInst);
      ICS (IMM_TABLE, ImmTableInst);
      ICS (IMM_MEMARG, ImmMemargInst);
      ICS (IMM_I32, ImmI32Inst);
      ICS (IMM_F64, ImmF64Inst);
      ICS (IMM_MEMORY, ImmMemoryInst);
      ICS (IMM_TAG, ImmTagInst);
      ICS (IMM_I64, ImmI64Inst);
      ICS (IMM_F32, ImmF32Inst);
      ICS (IMM_REFNULLT, ImmRefnulltInst);
      ICS (IMM_VALTS, ImmValtsInst);
      // Extension Immediates
      ICS (IMM_DATA_MEMORY, ImmDataMemoryInst);
      ICS (IMM_DATA, ImmDataInst);
      ICS (IMM_MEMORYCP, ImmMemorycpInst);
      ICS (IMM_DATA_TABLE, ImmDataTableInst);
      ICS (IMM_TABLECP, ImmTablecpInst);
      ICS (IMM_V128, ImmV128Inst);
      ICS (IMM_LANEIDX, ImmLaneidxInst);
      ICS (IMM_LANEIDX16, ImmLaneidx16Inst);
      ICS (IMM_MEMARG_LANEIDX, ImmMemargLaneidxInst);
      default:
        ERR("Unknown imm type: %d\n", imm_type);
        throw std::runtime_error("Unknown imm");
    }
    TRACE("Op: %20s | PC: %u\n", opcode_table[instptr->getOpcode()].mnemonic,
                              instptr->getOrigPc()); 
    return instptr;
  }
  #undef ICS
}