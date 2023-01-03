#pragma once
#include "common.h"
#include "wasmdefs.h"
#include "ir.h"

class InstBase {
  private:
    byte opcode;
  public:
    InstBase (byte opcode) : opcode(opcode) {};
    InstBase (buffer_t &buf) {
      RD_BYTE();
    }
    
    byte getOpcode() { return opcode; }
    virtual void decode_imm (buffer_t &buf) = 0;
};


class ImmNoneInst : public InstBase {
  public:
    ImmNoneInst (buffer_t &buf) : InstBase(RD_U32()) { }
    void decode_imm (buffer_t &buf) { }
};
