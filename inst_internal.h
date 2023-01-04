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
};


/* Immediate types intermediate class 
* Decode immediate is performed within constructor
* Encode immediate is a virtual method
* */
class ImmNoneInst : public InstBase {
  public:
    ImmNoneInst (byte opcode, buffer_t &buf);
};


class ImmBlocktInst: public InstBase {

  public:
    ImmBlocktInst (byte opcode, buffer_t &buf);
};


class ImmLabelInst: public InstBase {

  public:
    ImmLabelInst (byte opcode, buffer_t &buf);
};


class ImmLabelsInst: public InstBase {

  public:
    ImmLabelsInst (byte opcode, buffer_t &buf);
};


class ImmFuncInst: public InstBase {

  public:
    ImmFuncInst (byte opcode, buffer_t &buf);
};


class ImmSigTableInst: public InstBase {

  public:
    ImmSigTableInst (byte opcode, buffer_t &buf);
};


class ImmLocalInst: public InstBase {

  public:
    ImmLocalInst (byte opcode, buffer_t &buf);
};


class ImmGlobalInst: public InstBase {

  public:
    ImmGlobalInst (byte opcode, buffer_t &buf);
};


class ImmTableInst: public InstBase {

  public:
    ImmTableInst (byte opcode, buffer_t &buf);
};


class ImmMemargInst: public InstBase {

  public:
    ImmMemargInst (byte opcode, buffer_t &buf);
};


class ImmI32Inst: public InstBase {

  public:
    ImmI32Inst (byte opcode, buffer_t &buf);
};


class ImmF64Inst: public InstBase {

  public:
    ImmF64Inst (byte opcode, buffer_t &buf);
};


class ImmMemoryInst: public InstBase {

  public:
    ImmMemoryInst (byte opcode, buffer_t &buf);
};


class ImmTagInst: public InstBase {

  public:
    ImmTagInst (byte opcode, buffer_t &buf);
};


class ImmI64Inst: public InstBase {

  public:
    ImmI64Inst (byte opcode, buffer_t &buf);
};


class ImmF32Inst: public InstBase {

  public:
    ImmF32Inst (byte opcode, buffer_t &buf);
};


class ImmRefnulltInst: public InstBase {

  public:
    ImmRefnulltInst (byte opcode, buffer_t &buf);
};


class ImmValtsInst: public InstBase {

  public:
    ImmValtsInst (byte opcode, buffer_t &buf);
};


