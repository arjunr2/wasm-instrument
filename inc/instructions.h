#include "inst_internal.h"

class UnreachableInst : public ImmNoneInst {
  public:
    UnreachableInst() : ImmNoneInst(WASM_OP_UNREACHABLE) {}
};

class NopInst : public ImmNoneInst {
  public:
    NopInst() : ImmNoneInst(WASM_OP_NOP) {}
};

class BlockInst : public ImmBlocktInst {
  public:
    BlockInst(int64_t type) : ImmBlocktInst(WASM_OP_BLOCK, type) {}
};

class LoopInst : public ImmBlocktInst {
  public:
    LoopInst(int64_t type) : ImmBlocktInst(WASM_OP_LOOP, type) {}
};

class IfInst : public ImmBlocktInst {
  public:
    IfInst(int64_t type) : ImmBlocktInst(WASM_OP_IF, type) {}
};

class ElseInst : public ImmNoneInst {
  public:
    ElseInst() : ImmNoneInst(WASM_OP_ELSE) {}
};

class EndInst : public ImmNoneInst {
  public:
    EndInst() : ImmNoneInst(WASM_OP_END) {}
};

class BrInst : public ImmLabelInst {
  public:
    BrInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR, idx) {}
};

class BrIfInst : public ImmLabelInst {
  public:
    BrIfInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR_IF, idx) {}
};

class BrTableInst : public ImmLabelsInst {
  public:
    BrTableInst(std::list<uint32_t> idxs, uint32_t def_idx)
        : ImmLabelsInst(WASM_OP_BR_TABLE, idxs, def_idx) {}
};

class ReturnInst : public ImmNoneInst {
  public:
    ReturnInst() : ImmNoneInst(WASM_OP_RETURN) {}
};

class CallInst : public ImmFuncInst {
  public:
    CallInst(FuncDecl *func) : ImmFuncInst(WASM_OP_CALL, func) {}
};

class CallIndirectInst : public ImmSigTableInst {
  public:
    CallIndirectInst(SigDecl *sig, TableDecl *table)
        : ImmSigTableInst(WASM_OP_CALL_INDIRECT, sig, table) {}
};

class DropInst : public ImmNoneInst {
  public:
    DropInst() : ImmNoneInst(WASM_OP_DROP) {}
};

class SelectInst : public ImmNoneInst {
  public:
    SelectInst() : ImmNoneInst(WASM_OP_SELECT) {}
};

class LocalGetInst : public ImmLocalInst {
  public:
    LocalGetInst(uint32_t idx) : ImmLocalInst(WASM_OP_LOCAL_GET, idx) {}
};

class LocalSetInst : public ImmLocalInst {
  public:
    LocalSetInst(uint32_t idx) : ImmLocalInst(WASM_OP_LOCAL_SET, idx) {}
};

class LocalTeeInst : public ImmLocalInst {
  public:
    LocalTeeInst(uint32_t idx) : ImmLocalInst(WASM_OP_LOCAL_TEE, idx) {}
};

class GlobalGetInst : public ImmGlobalInst {
  public:
    GlobalGetInst(GlobalDecl *global)
        : ImmGlobalInst(WASM_OP_GLOBAL_GET, global) {}
};

class GlobalSetInst : public ImmGlobalInst {
  public:
    GlobalSetInst(GlobalDecl *global)
        : ImmGlobalInst(WASM_OP_GLOBAL_SET, global) {}
};

class I64LoadInst : public ImmMemargInst {
  public:
    I64LoadInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_LOAD, align, offset, mem) {}
};

class F32LoadInst : public ImmMemargInst {
  public:
    F32LoadInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_F32_LOAD, align, offset, mem) {}
};

class I32LoadInst : public ImmMemargInst {
  public:
    I32LoadInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_LOAD, align, offset, mem) {}
};

class F64LoadInst : public ImmMemargInst {
  public:
    F64LoadInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_F64_LOAD, align, offset, mem) {}
};

class I32Load8SInst : public ImmMemargInst {
  public:
    I32Load8SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_LOAD8_S, align, offset, mem) {}
};

class I32Load8UInst : public ImmMemargInst {
  public:
    I32Load8UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_LOAD8_U, align, offset, mem) {}
};

class I32Load16SInst : public ImmMemargInst {
  public:
    I32Load16SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_LOAD16_S, align, offset, mem) {}
};

class I32Load16UInst : public ImmMemargInst {
  public:
    I32Load16UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_LOAD16_U, align, offset, mem) {}
};

class I64Load8SInst : public ImmMemargInst {
  public:
    I64Load8SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_LOAD8_S, align, offset, mem) {}
};

class I64Load8UInst : public ImmMemargInst {
  public:
    I64Load8UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_LOAD8_U, align, offset, mem) {}
};

class I64Load16SInst : public ImmMemargInst {
  public:
    I64Load16SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_LOAD16_S, align, offset, mem) {}
};

class I64Load16UInst : public ImmMemargInst {
  public:
    I64Load16UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_LOAD16_U, align, offset, mem) {}
};

class I64Load32SInst : public ImmMemargInst {
  public:
    I64Load32SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_LOAD32_S, align, offset, mem) {}
};

class I64Load32UInst : public ImmMemargInst {
  public:
    I64Load32UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_LOAD32_U, align, offset, mem) {}
};

class I32StoreInst : public ImmMemargInst {
  public:
    I32StoreInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_STORE, align, offset, mem) {}
};

class I64StoreInst : public ImmMemargInst {
  public:
    I64StoreInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_STORE, align, offset, mem) {}
};

class F32StoreInst : public ImmMemargInst {
  public:
    F32StoreInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_F32_STORE, align, offset, mem) {}
};

class F64StoreInst : public ImmMemargInst {
  public:
    F64StoreInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_F64_STORE, align, offset, mem) {}
};

class I32Store8Inst : public ImmMemargInst {
  public:
    I32Store8Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_STORE8, align, offset, mem) {}
};

class I32Store16Inst : public ImmMemargInst {
  public:
    I32Store16Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_STORE16, align, offset, mem) {}
};

class I64Store8Inst : public ImmMemargInst {
  public:
    I64Store8Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_STORE8, align, offset, mem) {}
};

class I64Store16Inst : public ImmMemargInst {
  public:
    I64Store16Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_STORE16, align, offset, mem) {}
};

class I64Store32Inst : public ImmMemargInst {
  public:
    I64Store32Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_STORE32, align, offset, mem) {}
};

class MemorySizeInst : public ImmMemoryInst {
  public:
    MemorySizeInst(MemoryDecl *mem) : ImmMemoryInst(WASM_OP_MEMORY_SIZE, mem) {}
};

class MemoryGrowInst : public ImmMemoryInst {
  public:
    MemoryGrowInst(MemoryDecl *mem) : ImmMemoryInst(WASM_OP_MEMORY_GROW, mem) {}
};

class I32ConstInst : public ImmI32Inst {
  public:
    I32ConstInst(uint32_t value) : ImmI32Inst(WASM_OP_I32_CONST, value) {}
};

class I64ConstInst : public ImmI64Inst {
  public:
    I64ConstInst(int64_t value) : ImmI64Inst(WASM_OP_I64_CONST, value) {}
};

class F32ConstInst : public ImmF32Inst {
  public:
    F32ConstInst(float value) : ImmF32Inst(WASM_OP_F32_CONST, value) {}
};

class F64ConstInst : public ImmF64Inst {
  public:
    F64ConstInst(double value) : ImmF64Inst(WASM_OP_F64_CONST, value) {}
};

class I32EqzInst : public ImmNoneInst {
  public:
    I32EqzInst() : ImmNoneInst(WASM_OP_I32_EQZ) {}
};

class I32EqInst : public ImmNoneInst {
  public:
    I32EqInst() : ImmNoneInst(WASM_OP_I32_EQ) {}
};

class I32NeInst : public ImmNoneInst {
  public:
    I32NeInst() : ImmNoneInst(WASM_OP_I32_NE) {}
};

class I32LtSInst : public ImmNoneInst {
  public:
    I32LtSInst() : ImmNoneInst(WASM_OP_I32_LT_S) {}
};

class I32LtUInst : public ImmNoneInst {
  public:
    I32LtUInst() : ImmNoneInst(WASM_OP_I32_LT_U) {}
};

class I32GtSInst : public ImmNoneInst {
  public:
    I32GtSInst() : ImmNoneInst(WASM_OP_I32_GT_S) {}
};

class I32GtUInst : public ImmNoneInst {
  public:
    I32GtUInst() : ImmNoneInst(WASM_OP_I32_GT_U) {}
};

class I32LeSInst : public ImmNoneInst {
  public:
    I32LeSInst() : ImmNoneInst(WASM_OP_I32_LE_S) {}
};

class I32LeUInst : public ImmNoneInst {
  public:
    I32LeUInst() : ImmNoneInst(WASM_OP_I32_LE_U) {}
};

class I32GeSInst : public ImmNoneInst {
  public:
    I32GeSInst() : ImmNoneInst(WASM_OP_I32_GE_S) {}
};

class I32GeUInst : public ImmNoneInst {
  public:
    I32GeUInst() : ImmNoneInst(WASM_OP_I32_GE_U) {}
};

class I64EqzInst : public ImmNoneInst {
  public:
    I64EqzInst() : ImmNoneInst(WASM_OP_I64_EQZ) {}
};

class I64EqInst : public ImmNoneInst {
  public:
    I64EqInst() : ImmNoneInst(WASM_OP_I64_EQ) {}
};

class I64NeInst : public ImmNoneInst {
  public:
    I64NeInst() : ImmNoneInst(WASM_OP_I64_NE) {}
};

class I64LtSInst : public ImmNoneInst {
  public:
    I64LtSInst() : ImmNoneInst(WASM_OP_I64_LT_S) {}
};

class I64LtUInst : public ImmNoneInst {
  public:
    I64LtUInst() : ImmNoneInst(WASM_OP_I64_LT_U) {}
};

class I64GtSInst : public ImmNoneInst {
  public:
    I64GtSInst() : ImmNoneInst(WASM_OP_I64_GT_S) {}
};

class I64GtUInst : public ImmNoneInst {
  public:
    I64GtUInst() : ImmNoneInst(WASM_OP_I64_GT_U) {}
};

class I64LeSInst : public ImmNoneInst {
  public:
    I64LeSInst() : ImmNoneInst(WASM_OP_I64_LE_S) {}
};

class I64LeUInst : public ImmNoneInst {
  public:
    I64LeUInst() : ImmNoneInst(WASM_OP_I64_LE_U) {}
};

class I64GeSInst : public ImmNoneInst {
  public:
    I64GeSInst() : ImmNoneInst(WASM_OP_I64_GE_S) {}
};

class I64GeUInst : public ImmNoneInst {
  public:
    I64GeUInst() : ImmNoneInst(WASM_OP_I64_GE_U) {}
};

class F32EqInst : public ImmNoneInst {
  public:
    F32EqInst() : ImmNoneInst(WASM_OP_F32_EQ) {}
};

class F32NeInst : public ImmNoneInst {
  public:
    F32NeInst() : ImmNoneInst(WASM_OP_F32_NE) {}
};

class F32LtInst : public ImmNoneInst {
  public:
    F32LtInst() : ImmNoneInst(WASM_OP_F32_LT) {}
};

class F32GtInst : public ImmNoneInst {
  public:
    F32GtInst() : ImmNoneInst(WASM_OP_F32_GT) {}
};

class F32LeInst : public ImmNoneInst {
  public:
    F32LeInst() : ImmNoneInst(WASM_OP_F32_LE) {}
};

class F32GeInst : public ImmNoneInst {
  public:
    F32GeInst() : ImmNoneInst(WASM_OP_F32_GE) {}
};

class F64EqInst : public ImmNoneInst {
  public:
    F64EqInst() : ImmNoneInst(WASM_OP_F64_EQ) {}
};

class F64NeInst : public ImmNoneInst {
  public:
    F64NeInst() : ImmNoneInst(WASM_OP_F64_NE) {}
};

class F64LtInst : public ImmNoneInst {
  public:
    F64LtInst() : ImmNoneInst(WASM_OP_F64_LT) {}
};

class F64GtInst : public ImmNoneInst {
  public:
    F64GtInst() : ImmNoneInst(WASM_OP_F64_GT) {}
};

class F64LeInst : public ImmNoneInst {
  public:
    F64LeInst() : ImmNoneInst(WASM_OP_F64_LE) {}
};

class F64GeInst : public ImmNoneInst {
  public:
    F64GeInst() : ImmNoneInst(WASM_OP_F64_GE) {}
};

class I32ClzInst : public ImmNoneInst {
  public:
    I32ClzInst() : ImmNoneInst(WASM_OP_I32_CLZ) {}
};

class I32CtzInst : public ImmNoneInst {
  public:
    I32CtzInst() : ImmNoneInst(WASM_OP_I32_CTZ) {}
};

class I32PopcntInst : public ImmNoneInst {
  public:
    I32PopcntInst() : ImmNoneInst(WASM_OP_I32_POPCNT) {}
};

class I32AddInst : public ImmNoneInst {
  public:
    I32AddInst() : ImmNoneInst(WASM_OP_I32_ADD) {}
};

class I32SubInst : public ImmNoneInst {
  public:
    I32SubInst() : ImmNoneInst(WASM_OP_I32_SUB) {}
};

class I32MulInst : public ImmNoneInst {
  public:
    I32MulInst() : ImmNoneInst(WASM_OP_I32_MUL) {}
};

class I32DivSInst : public ImmNoneInst {
  public:
    I32DivSInst() : ImmNoneInst(WASM_OP_I32_DIV_S) {}
};

class I32DivUInst : public ImmNoneInst {
  public:
    I32DivUInst() : ImmNoneInst(WASM_OP_I32_DIV_U) {}
};

class I32RemSInst : public ImmNoneInst {
  public:
    I32RemSInst() : ImmNoneInst(WASM_OP_I32_REM_S) {}
};

class I32RemUInst : public ImmNoneInst {
  public:
    I32RemUInst() : ImmNoneInst(WASM_OP_I32_REM_U) {}
};

class I32AndInst : public ImmNoneInst {
  public:
    I32AndInst() : ImmNoneInst(WASM_OP_I32_AND) {}
};

class I32OrInst : public ImmNoneInst {
  public:
    I32OrInst() : ImmNoneInst(WASM_OP_I32_OR) {}
};

class I32XorInst : public ImmNoneInst {
  public:
    I32XorInst() : ImmNoneInst(WASM_OP_I32_XOR) {}
};

class I32ShlInst : public ImmNoneInst {
  public:
    I32ShlInst() : ImmNoneInst(WASM_OP_I32_SHL) {}
};

class I32ShrSInst : public ImmNoneInst {
  public:
    I32ShrSInst() : ImmNoneInst(WASM_OP_I32_SHR_S) {}
};

class I32ShrUInst : public ImmNoneInst {
  public:
    I32ShrUInst() : ImmNoneInst(WASM_OP_I32_SHR_U) {}
};

class I32RotlInst : public ImmNoneInst {
  public:
    I32RotlInst() : ImmNoneInst(WASM_OP_I32_ROTL) {}
};

class I32RotrInst : public ImmNoneInst {
  public:
    I32RotrInst() : ImmNoneInst(WASM_OP_I32_ROTR) {}
};

class I64ClzInst : public ImmNoneInst {
  public:
    I64ClzInst() : ImmNoneInst(WASM_OP_I64_CLZ) {}
};

class I64CtzInst : public ImmNoneInst {
  public:
    I64CtzInst() : ImmNoneInst(WASM_OP_I64_CTZ) {}
};

class I64PopcntInst : public ImmNoneInst {
  public:
    I64PopcntInst() : ImmNoneInst(WASM_OP_I64_POPCNT) {}
};

class I64AddInst : public ImmNoneInst {
  public:
    I64AddInst() : ImmNoneInst(WASM_OP_I64_ADD) {}
};

class I64SubInst : public ImmNoneInst {
  public:
    I64SubInst() : ImmNoneInst(WASM_OP_I64_SUB) {}
};

class I64MulInst : public ImmNoneInst {
  public:
    I64MulInst() : ImmNoneInst(WASM_OP_I64_MUL) {}
};

class I64DivSInst : public ImmNoneInst {
  public:
    I64DivSInst() : ImmNoneInst(WASM_OP_I64_DIV_S) {}
};

class I64DivUInst : public ImmNoneInst {
  public:
    I64DivUInst() : ImmNoneInst(WASM_OP_I64_DIV_U) {}
};

class I64RemSInst : public ImmNoneInst {
  public:
    I64RemSInst() : ImmNoneInst(WASM_OP_I64_REM_S) {}
};

class I64RemUInst : public ImmNoneInst {
  public:
    I64RemUInst() : ImmNoneInst(WASM_OP_I64_REM_U) {}
};

class I64AndInst : public ImmNoneInst {
  public:
    I64AndInst() : ImmNoneInst(WASM_OP_I64_AND) {}
};

class I64OrInst : public ImmNoneInst {
  public:
    I64OrInst() : ImmNoneInst(WASM_OP_I64_OR) {}
};

class I64XorInst : public ImmNoneInst {
  public:
    I64XorInst() : ImmNoneInst(WASM_OP_I64_XOR) {}
};

class I64ShlInst : public ImmNoneInst {
  public:
    I64ShlInst() : ImmNoneInst(WASM_OP_I64_SHL) {}
};

class I64ShrSInst : public ImmNoneInst {
  public:
    I64ShrSInst() : ImmNoneInst(WASM_OP_I64_SHR_S) {}
};

class I64ShrUInst : public ImmNoneInst {
  public:
    I64ShrUInst() : ImmNoneInst(WASM_OP_I64_SHR_U) {}
};

class I64RotlInst : public ImmNoneInst {
  public:
    I64RotlInst() : ImmNoneInst(WASM_OP_I64_ROTL) {}
};

class I64RotrInst : public ImmNoneInst {
  public:
    I64RotrInst() : ImmNoneInst(WASM_OP_I64_ROTR) {}
};

class F32AbsInst : public ImmNoneInst {
  public:
    F32AbsInst() : ImmNoneInst(WASM_OP_F32_ABS) {}
};

class F32NegInst : public ImmNoneInst {
  public:
    F32NegInst() : ImmNoneInst(WASM_OP_F32_NEG) {}
};

class F32CeilInst : public ImmNoneInst {
  public:
    F32CeilInst() : ImmNoneInst(WASM_OP_F32_CEIL) {}
};

class F32FloorInst : public ImmNoneInst {
  public:
    F32FloorInst() : ImmNoneInst(WASM_OP_F32_FLOOR) {}
};

class F32TruncInst : public ImmNoneInst {
  public:
    F32TruncInst() : ImmNoneInst(WASM_OP_F32_TRUNC) {}
};

class F32NearestInst : public ImmNoneInst {
  public:
    F32NearestInst() : ImmNoneInst(WASM_OP_F32_NEAREST) {}
};

class F32SqrtInst : public ImmNoneInst {
  public:
    F32SqrtInst() : ImmNoneInst(WASM_OP_F32_SQRT) {}
};

class F32AddInst : public ImmNoneInst {
  public:
    F32AddInst() : ImmNoneInst(WASM_OP_F32_ADD) {}
};

class F32SubInst : public ImmNoneInst {
  public:
    F32SubInst() : ImmNoneInst(WASM_OP_F32_SUB) {}
};

class F32MulInst : public ImmNoneInst {
  public:
    F32MulInst() : ImmNoneInst(WASM_OP_F32_MUL) {}
};

class F32DivInst : public ImmNoneInst {
  public:
    F32DivInst() : ImmNoneInst(WASM_OP_F32_DIV) {}
};

class F32MinInst : public ImmNoneInst {
  public:
    F32MinInst() : ImmNoneInst(WASM_OP_F32_MIN) {}
};

class F32MaxInst : public ImmNoneInst {
  public:
    F32MaxInst() : ImmNoneInst(WASM_OP_F32_MAX) {}
};

class F32CopysignInst : public ImmNoneInst {
  public:
    F32CopysignInst() : ImmNoneInst(WASM_OP_F32_COPYSIGN) {}
};

class F64AbsInst : public ImmNoneInst {
  public:
    F64AbsInst() : ImmNoneInst(WASM_OP_F64_ABS) {}
};

class F64NegInst : public ImmNoneInst {
  public:
    F64NegInst() : ImmNoneInst(WASM_OP_F64_NEG) {}
};

class F64CeilInst : public ImmNoneInst {
  public:
    F64CeilInst() : ImmNoneInst(WASM_OP_F64_CEIL) {}
};

class F64FloorInst : public ImmNoneInst {
  public:
    F64FloorInst() : ImmNoneInst(WASM_OP_F64_FLOOR) {}
};

class F64TruncInst : public ImmNoneInst {
  public:
    F64TruncInst() : ImmNoneInst(WASM_OP_F64_TRUNC) {}
};

class F64NearestInst : public ImmNoneInst {
  public:
    F64NearestInst() : ImmNoneInst(WASM_OP_F64_NEAREST) {}
};

class F64SqrtInst : public ImmNoneInst {
  public:
    F64SqrtInst() : ImmNoneInst(WASM_OP_F64_SQRT) {}
};

class F64AddInst : public ImmNoneInst {
  public:
    F64AddInst() : ImmNoneInst(WASM_OP_F64_ADD) {}
};

class F64SubInst : public ImmNoneInst {
  public:
    F64SubInst() : ImmNoneInst(WASM_OP_F64_SUB) {}
};

class F64MulInst : public ImmNoneInst {
  public:
    F64MulInst() : ImmNoneInst(WASM_OP_F64_MUL) {}
};

class F64DivInst : public ImmNoneInst {
  public:
    F64DivInst() : ImmNoneInst(WASM_OP_F64_DIV) {}
};

class F64MinInst : public ImmNoneInst {
  public:
    F64MinInst() : ImmNoneInst(WASM_OP_F64_MIN) {}
};

class F64MaxInst : public ImmNoneInst {
  public:
    F64MaxInst() : ImmNoneInst(WASM_OP_F64_MAX) {}
};

class F64CopysignInst : public ImmNoneInst {
  public:
    F64CopysignInst() : ImmNoneInst(WASM_OP_F64_COPYSIGN) {}
};

class I32WrapI64Inst : public ImmNoneInst {
  public:
    I32WrapI64Inst() : ImmNoneInst(WASM_OP_I32_WRAP_I64) {}
};

class I32TruncF32SInst : public ImmNoneInst {
  public:
    I32TruncF32SInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F32_S) {}
};

class I32TruncF32UInst : public ImmNoneInst {
  public:
    I32TruncF32UInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F32_U) {}
};

class I32TruncF64SInst : public ImmNoneInst {
  public:
    I32TruncF64SInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F64_S) {}
};

class I32TruncF64UInst : public ImmNoneInst {
  public:
    I32TruncF64UInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F64_U) {}
};

class I64ExtendI32SInst : public ImmNoneInst {
  public:
    I64ExtendI32SInst() : ImmNoneInst(WASM_OP_I64_EXTEND_I32_S) {}
};

class I64ExtendI32UInst : public ImmNoneInst {
  public:
    I64ExtendI32UInst() : ImmNoneInst(WASM_OP_I64_EXTEND_I32_U) {}
};

class I64TruncF32SInst : public ImmNoneInst {
  public:
    I64TruncF32SInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F32_S) {}
};

class I64TruncF32UInst : public ImmNoneInst {
  public:
    I64TruncF32UInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F32_U) {}
};

class I64TruncF64SInst : public ImmNoneInst {
  public:
    I64TruncF64SInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F64_S) {}
};

class I64TruncF64UInst : public ImmNoneInst {
  public:
    I64TruncF64UInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F64_U) {}
};

class F32ConvertI32SInst : public ImmNoneInst {
  public:
    F32ConvertI32SInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I32_S) {}
};

class F32ConvertI32UInst : public ImmNoneInst {
  public:
    F32ConvertI32UInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I32_U) {}
};

class F32ConvertI64SInst : public ImmNoneInst {
  public:
    F32ConvertI64SInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I64_S) {}
};

class F32ConvertI64UInst : public ImmNoneInst {
  public:
    F32ConvertI64UInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I64_U) {}
};

class F32DemoteF64Inst : public ImmNoneInst {
  public:
    F32DemoteF64Inst() : ImmNoneInst(WASM_OP_F32_DEMOTE_F64) {}
};

class F64ConvertI32SInst : public ImmNoneInst {
  public:
    F64ConvertI32SInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I32_S) {}
};

class F64ConvertI32UInst : public ImmNoneInst {
  public:
    F64ConvertI32UInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I32_U) {}
};

class F64ConvertI64SInst : public ImmNoneInst {
  public:
    F64ConvertI64SInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I64_S) {}
};

class F64ConvertI64UInst : public ImmNoneInst {
  public:
    F64ConvertI64UInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I64_U) {}
};

class F64PromoteF32Inst : public ImmNoneInst {
  public:
    F64PromoteF32Inst() : ImmNoneInst(WASM_OP_F64_PROMOTE_F32) {}
};

class I32ReinterpretF32Inst : public ImmNoneInst {
  public:
    I32ReinterpretF32Inst() : ImmNoneInst(WASM_OP_I32_REINTERPRET_F32) {}
};

class I64ReinterpretF64Inst : public ImmNoneInst {
  public:
    I64ReinterpretF64Inst() : ImmNoneInst(WASM_OP_I64_REINTERPRET_F64) {}
};

class F32ReinterpretI32Inst : public ImmNoneInst {
  public:
    F32ReinterpretI32Inst() : ImmNoneInst(WASM_OP_F32_REINTERPRET_I32) {}
};

class F64ReinterpretI64Inst : public ImmNoneInst {
  public:
    F64ReinterpretI64Inst() : ImmNoneInst(WASM_OP_F64_REINTERPRET_I64) {}
};

class I32Extend8SInst : public ImmNoneInst {
  public:
    I32Extend8SInst() : ImmNoneInst(WASM_OP_I32_EXTEND8_S) {}
};

class I32Extend16SInst : public ImmNoneInst {
  public:
    I32Extend16SInst() : ImmNoneInst(WASM_OP_I32_EXTEND16_S) {}
};

class I64Extend8SInst : public ImmNoneInst {
  public:
    I64Extend8SInst() : ImmNoneInst(WASM_OP_I64_EXTEND8_S) {}
};

class I64Extend16SInst : public ImmNoneInst {
  public:
    I64Extend16SInst() : ImmNoneInst(WASM_OP_I64_EXTEND16_S) {}
};

class I64Extend32SInst : public ImmNoneInst {
  public:
    I64Extend32SInst() : ImmNoneInst(WASM_OP_I64_EXTEND32_S) {}
};

class TryInst : public ImmBlocktInst {
  public:
    TryInst(int64_t type) : ImmBlocktInst(WASM_OP_TRY, type) {}
};

class CatchInst : public ImmTagInst {
  public:
    CatchInst() : ImmTagInst(WASM_OP_CATCH) {}
};

class ThrowInst : public ImmTagInst {
  public:
    ThrowInst() : ImmTagInst(WASM_OP_THROW) {}
};

class RethrowInst : public ImmNoneInst {
  public:
    RethrowInst() : ImmNoneInst(WASM_OP_RETHROW) {}
};

class ReturnCallInst : public ImmFuncInst {
  public:
    ReturnCallInst(FuncDecl *func) : ImmFuncInst(WASM_OP_RETURN_CALL, func) {}
};

class ReturnCallIndirectInst : public ImmSigTableInst {
  public:
    ReturnCallIndirectInst(SigDecl *sig, TableDecl *table)
        : ImmSigTableInst(WASM_OP_RETURN_CALL_INDIRECT, sig, table) {}
};

class CallRefInst : public ImmNoneInst {
  public:
    CallRefInst() : ImmNoneInst(WASM_OP_CALL_REF) {}
};

class ReturnCallRefInst : public ImmNoneInst {
  public:
    ReturnCallRefInst() : ImmNoneInst(WASM_OP_RETURN_CALL_REF) {}
};

class DelegateInst : public ImmNoneInst {
  public:
    DelegateInst() : ImmNoneInst(WASM_OP_DELEGATE) {}
};

class CatchAllInst : public ImmNoneInst {
  public:
    CatchAllInst() : ImmNoneInst(WASM_OP_CATCH_ALL) {}
};

class SelectTInst : public ImmValtsInst {
  public:
    SelectTInst(std::list<wasm_type_t> types)
        : ImmValtsInst(WASM_OP_SELECT_T, types) {}
};

class TableGetInst : public ImmTableInst {
  public:
    TableGetInst(TableDecl *table) : ImmTableInst(WASM_OP_TABLE_GET, table) {}
};

class TableSetInst : public ImmTableInst {
  public:
    TableSetInst(TableDecl *table) : ImmTableInst(WASM_OP_TABLE_SET, table) {}
};

class RefNullInst : public ImmRefnulltInst {
  public:
    RefNullInst(wasm_type_t type) : ImmRefnulltInst(WASM_OP_REF_NULL, type) {}
};

class RefIsNullInst : public ImmNoneInst {
  public:
    RefIsNullInst() : ImmNoneInst(WASM_OP_REF_IS_NULL) {}
};

class RefFuncInst : public ImmFuncInst {
  public:
    RefFuncInst(FuncDecl *func) : ImmFuncInst(WASM_OP_REF_FUNC, func) {}
};

class RefAsNonNullInst : public ImmNoneInst {
  public:
    RefAsNonNullInst() : ImmNoneInst(WASM_OP_REF_AS_NON_NULL) {}
};

class BrOnNullInst : public ImmLabelInst {
  public:
    BrOnNullInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR_ON_NULL, idx) {}
};

class RefEqInst : public ImmNoneInst {
  public:
    RefEqInst() : ImmNoneInst(WASM_OP_REF_EQ) {}
};

class BrOnNonNullInst : public ImmLabelInst {
  public:
    BrOnNonNullInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR_ON_NON_NULL, idx) {}
};

class MemoryInitInst : public ImmDataMemoryInst {
  public:
    MemoryInitInst(DataDecl *data, MemoryDecl *mem)
        : ImmDataMemoryInst(WASM_OP_MEMORY_INIT, data, mem) {}
};

class DataDropInst : public ImmDataInst {
  public:
    DataDropInst(DataDecl *data) : ImmDataInst(WASM_OP_DATA_DROP, data) {}
};

class MemoryCopyInst : public ImmMemorycpInst {
  public:
    MemoryCopyInst(MemoryDecl *dst, MemoryDecl *src)
        : ImmMemorycpInst(WASM_OP_MEMORY_COPY, dst, src) {}
};

class MemoryFillInst : public ImmMemoryInst {
  public:
    MemoryFillInst(MemoryDecl *mem) : ImmMemoryInst(WASM_OP_MEMORY_FILL, mem) {}
};

class V128LoadInst : public ImmMemargInst {
  public:
    V128LoadInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD, align, offset, mem) {}
};

class V128Load8x8SInst : public ImmMemargInst {
  public:
    V128Load8x8SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD8x8_S, align, offset, mem) {}
};

class V128Load8x8UInst : public ImmMemargInst {
  public:
    V128Load8x8UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD8x8_U, align, offset, mem) {}
};

class V128Load16x4SInst : public ImmMemargInst {
  public:
    V128Load16x4SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD16x4_S, align, offset, mem) {}
};

class V128Load16x4UInst : public ImmMemargInst {
  public:
    V128Load16x4UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD16x4_U, align, offset, mem) {}
};

class V128Load32x2SInst : public ImmMemargInst {
  public:
    V128Load32x2SInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD32x2_S, align, offset, mem) {}
};

class V128Load32x2UInst : public ImmMemargInst {
  public:
    V128Load32x2UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD32x2_U, align, offset, mem) {}
};

class V128Load8SplatInst : public ImmMemargInst {
  public:
    V128Load8SplatInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD8_SPLAT, align, offset, mem) {}
};

class V128Load16SplatInst : public ImmMemargInst {
  public:
    V128Load16SplatInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD16_SPLAT, align, offset, mem) {}
};

class V128Load32SplatInst : public ImmMemargInst {
  public:
    V128Load32SplatInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD32_SPLAT, align, offset, mem) {}
};

class V128Load64SplatInst : public ImmMemargInst {
  public:
    V128Load64SplatInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD64_SPLAT, align, offset, mem) {}
};

class V128StoreInst : public ImmMemargInst {
  public:
    V128StoreInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_STORE, align, offset, mem) {}
};

class V128ConstInst : public ImmV128Inst {
  public:
    V128ConstInst(V128_t value) : ImmV128Inst(WASM_OP_V128_CONST, value) {}
};

class I8x16ShuffleInst : public ImmLaneidx16Inst {
  public:
    I8x16ShuffleInst(Laneidx16_t idxs)
        : ImmLaneidx16Inst(WASM_OP_I8x16_SHUFFLE, idxs) {}
};

class I8x16SwizzleInst : public ImmNoneInst {
  public:
    I8x16SwizzleInst() : ImmNoneInst(WASM_OP_I8x16_SWIZZLE) {}
};

class I8x16SplatInst : public ImmNoneInst {
  public:
    I8x16SplatInst() : ImmNoneInst(WASM_OP_I8x16_SPLAT) {}
};

class I16x8SplatInst : public ImmNoneInst {
  public:
    I16x8SplatInst() : ImmNoneInst(WASM_OP_I16x8_SPLAT) {}
};

class I32x4SplatInst : public ImmNoneInst {
  public:
    I32x4SplatInst() : ImmNoneInst(WASM_OP_I32x4_SPLAT) {}
};

class I64x2SplatInst : public ImmNoneInst {
  public:
    I64x2SplatInst() : ImmNoneInst(WASM_OP_I64x2_SPLAT) {}
};

class F32x4SplatInst : public ImmNoneInst {
  public:
    F32x4SplatInst() : ImmNoneInst(WASM_OP_F32x4_SPLAT) {}
};

class F64x2SplatInst : public ImmNoneInst {
  public:
    F64x2SplatInst() : ImmNoneInst(WASM_OP_F64x2_SPLAT) {}
};

class I8x16ExtractLaneSInst : public ImmLaneidxInst {
  public:
    I8x16ExtractLaneSInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I8x16_EXTRACT_LANE_S, idx) {}
};

class I8x16ExtractLaneUInst : public ImmLaneidxInst {
  public:
    I8x16ExtractLaneUInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I8x16_EXTRACT_LANE_U, idx) {}
};

class I8x16ReplaceLaneInst : public ImmLaneidxInst {
  public:
    I8x16ReplaceLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I8x16_REPLACE_LANE, idx) {}
};

class I16x8ExtractLaneSInst : public ImmLaneidxInst {
  public:
    I16x8ExtractLaneSInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I16x8_EXTRACT_LANE_S, idx) {}
};

class I16x8ExtractLaneUInst : public ImmLaneidxInst {
  public:
    I16x8ExtractLaneUInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I16x8_EXTRACT_LANE_U, idx) {}
};

class I16x8ReplaceLaneInst : public ImmLaneidxInst {
  public:
    I16x8ReplaceLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I16x8_REPLACE_LANE, idx) {}
};

class I32x4ExtractLaneInst : public ImmLaneidxInst {
  public:
    I32x4ExtractLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I32x4_EXTRACT_LANE, idx) {}
};

class I32x4ReplaceLaneInst : public ImmLaneidxInst {
  public:
    I32x4ReplaceLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I32x4_REPLACE_LANE, idx) {}
};

class I64x2ExtractLaneInst : public ImmLaneidxInst {
  public:
    I64x2ExtractLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I64x2_EXTRACT_LANE, idx) {}
};

class I64x2ReplaceLaneInst : public ImmLaneidxInst {
  public:
    I64x2ReplaceLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_I64x2_REPLACE_LANE, idx) {}
};

class F32x4ExtractLaneInst : public ImmLaneidxInst {
  public:
    F32x4ExtractLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_F32x4_EXTRACT_LANE, idx) {}
};

class F32x4ReplaceLaneInst : public ImmLaneidxInst {
  public:
    F32x4ReplaceLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_F32x4_REPLACE_LANE, idx) {}
};

class F64x2ExtractLaneInst : public ImmLaneidxInst {
  public:
    F64x2ExtractLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_F64x2_EXTRACT_LANE, idx) {}
};

class F64x2ReplaceLaneInst : public ImmLaneidxInst {
  public:
    F64x2ReplaceLaneInst(Laneidx_t idx)
        : ImmLaneidxInst(WASM_OP_F64x2_REPLACE_LANE, idx) {}
};

class I8x16EqInst : public ImmNoneInst {
  public:
    I8x16EqInst() : ImmNoneInst(WASM_OP_I8x16_EQ) {}
};

class I8x16NeInst : public ImmNoneInst {
  public:
    I8x16NeInst() : ImmNoneInst(WASM_OP_I8x16_NE) {}
};

class I8x16LtSInst : public ImmNoneInst {
  public:
    I8x16LtSInst() : ImmNoneInst(WASM_OP_I8x16_LT_S) {}
};

class I8x16LtUInst : public ImmNoneInst {
  public:
    I8x16LtUInst() : ImmNoneInst(WASM_OP_I8x16_LT_U) {}
};

class I8x16GtSInst : public ImmNoneInst {
  public:
    I8x16GtSInst() : ImmNoneInst(WASM_OP_I8x16_GT_S) {}
};

class I8x16GtUInst : public ImmNoneInst {
  public:
    I8x16GtUInst() : ImmNoneInst(WASM_OP_I8x16_GT_U) {}
};

class I8x16LeSInst : public ImmNoneInst {
  public:
    I8x16LeSInst() : ImmNoneInst(WASM_OP_I8x16_LE_S) {}
};

class I8x16LeUInst : public ImmNoneInst {
  public:
    I8x16LeUInst() : ImmNoneInst(WASM_OP_I8x16_LE_U) {}
};

class I8x16GeSInst : public ImmNoneInst {
  public:
    I8x16GeSInst() : ImmNoneInst(WASM_OP_I8x16_GE_S) {}
};

class I8x16GeUInst : public ImmNoneInst {
  public:
    I8x16GeUInst() : ImmNoneInst(WASM_OP_I8x16_GE_U) {}
};

class I16x8EqInst : public ImmNoneInst {
  public:
    I16x8EqInst() : ImmNoneInst(WASM_OP_I16x8_EQ) {}
};

class I16x8NeInst : public ImmNoneInst {
  public:
    I16x8NeInst() : ImmNoneInst(WASM_OP_I16x8_NE) {}
};

class I16x8LtSInst : public ImmNoneInst {
  public:
    I16x8LtSInst() : ImmNoneInst(WASM_OP_I16x8_LT_S) {}
};

class I16x8LtUInst : public ImmNoneInst {
  public:
    I16x8LtUInst() : ImmNoneInst(WASM_OP_I16x8_LT_U) {}
};

class I16x8GtSInst : public ImmNoneInst {
  public:
    I16x8GtSInst() : ImmNoneInst(WASM_OP_I16x8_GT_S) {}
};

class I16x8GtUInst : public ImmNoneInst {
  public:
    I16x8GtUInst() : ImmNoneInst(WASM_OP_I16x8_GT_U) {}
};

class I16x8LeSInst : public ImmNoneInst {
  public:
    I16x8LeSInst() : ImmNoneInst(WASM_OP_I16x8_LE_S) {}
};

class I16x8LeUInst : public ImmNoneInst {
  public:
    I16x8LeUInst() : ImmNoneInst(WASM_OP_I16x8_LE_U) {}
};

class I16x8GeSInst : public ImmNoneInst {
  public:
    I16x8GeSInst() : ImmNoneInst(WASM_OP_I16x8_GE_S) {}
};

class I16x8GeUInst : public ImmNoneInst {
  public:
    I16x8GeUInst() : ImmNoneInst(WASM_OP_I16x8_GE_U) {}
};

class I32x4EqInst : public ImmNoneInst {
  public:
    I32x4EqInst() : ImmNoneInst(WASM_OP_I32x4_EQ) {}
};

class I32x4NeInst : public ImmNoneInst {
  public:
    I32x4NeInst() : ImmNoneInst(WASM_OP_I32x4_NE) {}
};

class I32x4LtSInst : public ImmNoneInst {
  public:
    I32x4LtSInst() : ImmNoneInst(WASM_OP_I32x4_LT_S) {}
};

class I32x4LtUInst : public ImmNoneInst {
  public:
    I32x4LtUInst() : ImmNoneInst(WASM_OP_I32x4_LT_U) {}
};

class I32x4GtSInst : public ImmNoneInst {
  public:
    I32x4GtSInst() : ImmNoneInst(WASM_OP_I32x4_GT_S) {}
};

class I32x4GtUInst : public ImmNoneInst {
  public:
    I32x4GtUInst() : ImmNoneInst(WASM_OP_I32x4_GT_U) {}
};

class I32x4LeSInst : public ImmNoneInst {
  public:
    I32x4LeSInst() : ImmNoneInst(WASM_OP_I32x4_LE_S) {}
};

class I32x4LeUInst : public ImmNoneInst {
  public:
    I32x4LeUInst() : ImmNoneInst(WASM_OP_I32x4_LE_U) {}
};

class I32x4GeSInst : public ImmNoneInst {
  public:
    I32x4GeSInst() : ImmNoneInst(WASM_OP_I32x4_GE_S) {}
};

class I32x4GeUInst : public ImmNoneInst {
  public:
    I32x4GeUInst() : ImmNoneInst(WASM_OP_I32x4_GE_U) {}
};

class F32x4EqInst : public ImmNoneInst {
  public:
    F32x4EqInst() : ImmNoneInst(WASM_OP_F32x4_EQ) {}
};

class F32x4NeInst : public ImmNoneInst {
  public:
    F32x4NeInst() : ImmNoneInst(WASM_OP_F32x4_NE) {}
};

class F32x4LtInst : public ImmNoneInst {
  public:
    F32x4LtInst() : ImmNoneInst(WASM_OP_F32x4_LT) {}
};

class F32x4GtInst : public ImmNoneInst {
  public:
    F32x4GtInst() : ImmNoneInst(WASM_OP_F32x4_GT) {}
};

class F32x4LeInst : public ImmNoneInst {
  public:
    F32x4LeInst() : ImmNoneInst(WASM_OP_F32x4_LE) {}
};

class F32x4GeInst : public ImmNoneInst {
  public:
    F32x4GeInst() : ImmNoneInst(WASM_OP_F32x4_GE) {}
};

class F64x2EqInst : public ImmNoneInst {
  public:
    F64x2EqInst() : ImmNoneInst(WASM_OP_F64x2_EQ) {}
};

class F64x2NeInst : public ImmNoneInst {
  public:
    F64x2NeInst() : ImmNoneInst(WASM_OP_F64x2_NE) {}
};

class F64x2LtInst : public ImmNoneInst {
  public:
    F64x2LtInst() : ImmNoneInst(WASM_OP_F64x2_LT) {}
};

class F64x2GtInst : public ImmNoneInst {
  public:
    F64x2GtInst() : ImmNoneInst(WASM_OP_F64x2_GT) {}
};

class F64x2LeInst : public ImmNoneInst {
  public:
    F64x2LeInst() : ImmNoneInst(WASM_OP_F64x2_LE) {}
};

class F64x2GeInst : public ImmNoneInst {
  public:
    F64x2GeInst() : ImmNoneInst(WASM_OP_F64x2_GE) {}
};

class V128NotInst : public ImmNoneInst {
  public:
    V128NotInst() : ImmNoneInst(WASM_OP_V128_NOT) {}
};

class V128AndInst : public ImmNoneInst {
  public:
    V128AndInst() : ImmNoneInst(WASM_OP_V128_AND) {}
};

class V128AndnotInst : public ImmNoneInst {
  public:
    V128AndnotInst() : ImmNoneInst(WASM_OP_V128_ANDNOT) {}
};

class V128OrInst : public ImmNoneInst {
  public:
    V128OrInst() : ImmNoneInst(WASM_OP_V128_OR) {}
};

class V128XorInst : public ImmNoneInst {
  public:
    V128XorInst() : ImmNoneInst(WASM_OP_V128_XOR) {}
};

class V128BitselectInst : public ImmNoneInst {
  public:
    V128BitselectInst() : ImmNoneInst(WASM_OP_V128_BITSELECT) {}
};

class V128AnyTrueInst : public ImmNoneInst {
  public:
    V128AnyTrueInst() : ImmNoneInst(WASM_OP_V128_ANY_TRUE) {}
};

class V128Load8LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Load8LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                      Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_LOAD8_LANE, align, offset, mem,
                               idx) {}
};

class V128Load16LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Load16LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                       Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_LOAD16_LANE, align, offset, mem,
                               idx) {}
};

class V128Load32LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Load32LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                       Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_LOAD32_LANE, align, offset, mem,
                               idx) {}
};

class V128Load64LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Load64LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                       Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_LOAD64_LANE, align, offset, mem,
                               idx) {}
};

class V128Store8LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Store8LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                       Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_STORE8_LANE, align, offset, mem,
                               idx) {}
};

class V128Store16LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Store16LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                        Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_STORE16_LANE, align, offset, mem,
                               idx) {}
};

class V128Store32LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Store32LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                        Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_STORE32_LANE, align, offset, mem,
                               idx) {}
};

class V128Store64LaneInst : public ImmMemargLaneidxInst {
  public:
    V128Store64LaneInst(uint32_t align, uint32_t offset, MemoryDecl *mem,
                        Laneidx_t idx)
        : ImmMemargLaneidxInst(WASM_OP_V128_STORE64_LANE, align, offset, mem,
                               idx) {}
};

class V128Load32ZeroInst : public ImmMemargInst {
  public:
    V128Load32ZeroInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD32_ZERO, align, offset, mem) {}
};

class V128Load64ZeroInst : public ImmMemargInst {
  public:
    V128Load64ZeroInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_V128_LOAD64_ZERO, align, offset, mem) {}
};

class F32x4DemoteF64x2ZeroInst : public ImmNoneInst {
  public:
    F32x4DemoteF64x2ZeroInst() : ImmNoneInst(WASM_OP_F32x4_DEMOTE_F64x2_ZERO) {}
};

class F64x2PromoteF32x4Inst : public ImmNoneInst {
  public:
    F64x2PromoteF32x4Inst() : ImmNoneInst(WASM_OP_F64x2_PROMOTE_F32x4) {}
};

class I8x16AbsInst : public ImmNoneInst {
  public:
    I8x16AbsInst() : ImmNoneInst(WASM_OP_I8x16_ABS) {}
};

class I8x16NegInst : public ImmNoneInst {
  public:
    I8x16NegInst() : ImmNoneInst(WASM_OP_I8x16_NEG) {}
};

class I8x16PopcntInst : public ImmNoneInst {
  public:
    I8x16PopcntInst() : ImmNoneInst(WASM_OP_I8x16_POPCNT) {}
};

class I8x16AllTrueInst : public ImmNoneInst {
  public:
    I8x16AllTrueInst() : ImmNoneInst(WASM_OP_I8x16_ALL_TRUE) {}
};

class I8x16BitmaskInst : public ImmNoneInst {
  public:
    I8x16BitmaskInst() : ImmNoneInst(WASM_OP_I8x16_BITMASK) {}
};

class I8x16NarrowI16x8SInst : public ImmNoneInst {
  public:
    I8x16NarrowI16x8SInst() : ImmNoneInst(WASM_OP_I8x16_NARROW_I16x8_S) {}
};

class I8x16NarrowI16x8UInst : public ImmNoneInst {
  public:
    I8x16NarrowI16x8UInst() : ImmNoneInst(WASM_OP_I8x16_NARROW_I16x8_U) {}
};

class F32x4CeilInst : public ImmNoneInst {
  public:
    F32x4CeilInst() : ImmNoneInst(WASM_OP_F32x4_CEIL) {}
};

class F32x4FloorInst : public ImmNoneInst {
  public:
    F32x4FloorInst() : ImmNoneInst(WASM_OP_F32x4_FLOOR) {}
};

class F32x4TruncInst : public ImmNoneInst {
  public:
    F32x4TruncInst() : ImmNoneInst(WASM_OP_F32x4_TRUNC) {}
};

class F32x4NearestInst : public ImmNoneInst {
  public:
    F32x4NearestInst() : ImmNoneInst(WASM_OP_F32x4_NEAREST) {}
};

class I8x16ShlInst : public ImmNoneInst {
  public:
    I8x16ShlInst() : ImmNoneInst(WASM_OP_I8x16_SHL) {}
};

class I8x16ShrSInst : public ImmNoneInst {
  public:
    I8x16ShrSInst() : ImmNoneInst(WASM_OP_I8x16_SHR_S) {}
};

class I8x16ShrUInst : public ImmNoneInst {
  public:
    I8x16ShrUInst() : ImmNoneInst(WASM_OP_I8x16_SHR_U) {}
};

class I8x16AddInst : public ImmNoneInst {
  public:
    I8x16AddInst() : ImmNoneInst(WASM_OP_I8x16_ADD) {}
};

class I8x16AddSatSInst : public ImmNoneInst {
  public:
    I8x16AddSatSInst() : ImmNoneInst(WASM_OP_I8x16_ADD_SAT_S) {}
};

class I8x16AddSatUInst : public ImmNoneInst {
  public:
    I8x16AddSatUInst() : ImmNoneInst(WASM_OP_I8x16_ADD_SAT_U) {}
};

class I8x16SubInst : public ImmNoneInst {
  public:
    I8x16SubInst() : ImmNoneInst(WASM_OP_I8x16_SUB) {}
};

class I8x16SubSatSInst : public ImmNoneInst {
  public:
    I8x16SubSatSInst() : ImmNoneInst(WASM_OP_I8x16_SUB_SAT_S) {}
};

class I8x16SubSatUInst : public ImmNoneInst {
  public:
    I8x16SubSatUInst() : ImmNoneInst(WASM_OP_I8x16_SUB_SAT_U) {}
};

class F64x2CeilInst : public ImmNoneInst {
  public:
    F64x2CeilInst() : ImmNoneInst(WASM_OP_F64x2_CEIL) {}
};

class F64x2FloorInst : public ImmNoneInst {
  public:
    F64x2FloorInst() : ImmNoneInst(WASM_OP_F64x2_FLOOR) {}
};

class I8x16MinSInst : public ImmNoneInst {
  public:
    I8x16MinSInst() : ImmNoneInst(WASM_OP_I8x16_MIN_S) {}
};

class I8x16MinUInst : public ImmNoneInst {
  public:
    I8x16MinUInst() : ImmNoneInst(WASM_OP_I8x16_MIN_U) {}
};

class I8x16MaxSInst : public ImmNoneInst {
  public:
    I8x16MaxSInst() : ImmNoneInst(WASM_OP_I8x16_MAX_S) {}
};

class I8x16MaxUInst : public ImmNoneInst {
  public:
    I8x16MaxUInst() : ImmNoneInst(WASM_OP_I8x16_MAX_U) {}
};

class F64x2TruncInst : public ImmNoneInst {
  public:
    F64x2TruncInst() : ImmNoneInst(WASM_OP_F64x2_TRUNC) {}
};

class I8x16AvgrUInst : public ImmNoneInst {
  public:
    I8x16AvgrUInst() : ImmNoneInst(WASM_OP_I8x16_AVGR_U) {}
};

class I16x8ExtaddPairwiseI8x16SInst : public ImmNoneInst {
  public:
    I16x8ExtaddPairwiseI8x16SInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTADD_PAIRWISE_I8x16_S) {}
};

class I16x8ExtaddPairwiseI8x16UInst : public ImmNoneInst {
  public:
    I16x8ExtaddPairwiseI8x16UInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTADD_PAIRWISE_I8x16_U) {}
};

class I32x4ExtaddPairwiseI16x8SInst : public ImmNoneInst {
  public:
    I32x4ExtaddPairwiseI16x8SInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTADD_PAIRWISE_I16x8_S) {}
};

class I32x4ExtaddPairwiseI16x8UInst : public ImmNoneInst {
  public:
    I32x4ExtaddPairwiseI16x8UInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTADD_PAIRWISE_I16x8_U) {}
};

class I16x8AbsInst : public ImmNoneInst {
  public:
    I16x8AbsInst() : ImmNoneInst(WASM_OP_I16x8_ABS) {}
};

class I16x8NegInst : public ImmNoneInst {
  public:
    I16x8NegInst() : ImmNoneInst(WASM_OP_I16x8_NEG) {}
};

class I16x8Q15mulrSatSInst : public ImmNoneInst {
  public:
    I16x8Q15mulrSatSInst() : ImmNoneInst(WASM_OP_I16x8_Q15MULR_SAT_S) {}
};

class I16x8AllTrueInst : public ImmNoneInst {
  public:
    I16x8AllTrueInst() : ImmNoneInst(WASM_OP_I16x8_ALL_TRUE) {}
};

class I16x8BitmaskInst : public ImmNoneInst {
  public:
    I16x8BitmaskInst() : ImmNoneInst(WASM_OP_I16x8_BITMASK) {}
};

class I16x8NarrowI32x4SInst : public ImmNoneInst {
  public:
    I16x8NarrowI32x4SInst() : ImmNoneInst(WASM_OP_I16x8_NARROW_I32x4_S) {}
};

class I16x8NarrowI32x4UInst : public ImmNoneInst {
  public:
    I16x8NarrowI32x4UInst() : ImmNoneInst(WASM_OP_I16x8_NARROW_I32x4_U) {}
};

class I16x8ExtendLowI8x16SInst : public ImmNoneInst {
  public:
    I16x8ExtendLowI8x16SInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTEND_LOW_I8x16_S) {}
};

class I16x8ExtendHighI8x16SInst : public ImmNoneInst {
  public:
    I16x8ExtendHighI8x16SInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTEND_HIGH_I8x16_S) {}
};

class I16x8ExtendLowI8x16UInst : public ImmNoneInst {
  public:
    I16x8ExtendLowI8x16UInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTEND_LOW_I8x16_U) {}
};

class I16x8ExtendHighI8x16UInst : public ImmNoneInst {
  public:
    I16x8ExtendHighI8x16UInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTEND_HIGH_I8x16_U) {}
};

class I16x8ShlInst : public ImmNoneInst {
  public:
    I16x8ShlInst() : ImmNoneInst(WASM_OP_I16x8_SHL) {}
};

class I16x8ShrSInst : public ImmNoneInst {
  public:
    I16x8ShrSInst() : ImmNoneInst(WASM_OP_I16x8_SHR_S) {}
};

class I16x8ShrUInst : public ImmNoneInst {
  public:
    I16x8ShrUInst() : ImmNoneInst(WASM_OP_I16x8_SHR_U) {}
};

class I16x8AddInst : public ImmNoneInst {
  public:
    I16x8AddInst() : ImmNoneInst(WASM_OP_I16x8_ADD) {}
};

class I16x8AddSatSInst : public ImmNoneInst {
  public:
    I16x8AddSatSInst() : ImmNoneInst(WASM_OP_I16x8_ADD_SAT_S) {}
};

class I16x8AddSatUInst : public ImmNoneInst {
  public:
    I16x8AddSatUInst() : ImmNoneInst(WASM_OP_I16x8_ADD_SAT_U) {}
};

class I16x8SubInst : public ImmNoneInst {
  public:
    I16x8SubInst() : ImmNoneInst(WASM_OP_I16x8_SUB) {}
};

class I16x8SubSatSInst : public ImmNoneInst {
  public:
    I16x8SubSatSInst() : ImmNoneInst(WASM_OP_I16x8_SUB_SAT_S) {}
};

class I16x8SubSatUInst : public ImmNoneInst {
  public:
    I16x8SubSatUInst() : ImmNoneInst(WASM_OP_I16x8_SUB_SAT_U) {}
};

class F64x2NearestInst : public ImmNoneInst {
  public:
    F64x2NearestInst() : ImmNoneInst(WASM_OP_F64x2_NEAREST) {}
};

class I16x8MulInst : public ImmNoneInst {
  public:
    I16x8MulInst() : ImmNoneInst(WASM_OP_I16x8_MUL) {}
};

class I16x8MinSInst : public ImmNoneInst {
  public:
    I16x8MinSInst() : ImmNoneInst(WASM_OP_I16x8_MIN_S) {}
};

class I16x8MinUInst : public ImmNoneInst {
  public:
    I16x8MinUInst() : ImmNoneInst(WASM_OP_I16x8_MIN_U) {}
};

class I16x8MaxSInst : public ImmNoneInst {
  public:
    I16x8MaxSInst() : ImmNoneInst(WASM_OP_I16x8_MAX_S) {}
};

class I16x8MaxUInst : public ImmNoneInst {
  public:
    I16x8MaxUInst() : ImmNoneInst(WASM_OP_I16x8_MAX_U) {}
};

class I16x8AvgrUInst : public ImmNoneInst {
  public:
    I16x8AvgrUInst() : ImmNoneInst(WASM_OP_I16x8_AVGR_U) {}
};

class I16x8ExtmulLowI8x16SInst : public ImmNoneInst {
  public:
    I16x8ExtmulLowI8x16SInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTMUL_LOW_I8x16_S) {}
};

class I16x8ExtmulHighI8x16SInst : public ImmNoneInst {
  public:
    I16x8ExtmulHighI8x16SInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTMUL_HIGH_I8x16_S) {}
};

class I16x8ExtmulLowI8x16UInst : public ImmNoneInst {
  public:
    I16x8ExtmulLowI8x16UInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTMUL_LOW_I8x16_U) {}
};

class I16x8ExtmulHighI8x16UInst : public ImmNoneInst {
  public:
    I16x8ExtmulHighI8x16UInst()
        : ImmNoneInst(WASM_OP_I16x8_EXTMUL_HIGH_I8x16_U) {}
};

class I32x4AbsInst : public ImmNoneInst {
  public:
    I32x4AbsInst() : ImmNoneInst(WASM_OP_I32x4_ABS) {}
};

class I32x4NegInst : public ImmNoneInst {
  public:
    I32x4NegInst() : ImmNoneInst(WASM_OP_I32x4_NEG) {}
};

class I32x4AllTrueInst : public ImmNoneInst {
  public:
    I32x4AllTrueInst() : ImmNoneInst(WASM_OP_I32x4_ALL_TRUE) {}
};

class I32x4BitmaskInst : public ImmNoneInst {
  public:
    I32x4BitmaskInst() : ImmNoneInst(WASM_OP_I32x4_BITMASK) {}
};

class I32x4ExtendLowI16x8SInst : public ImmNoneInst {
  public:
    I32x4ExtendLowI16x8SInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTEND_LOW_I16x8_S) {}
};

class I32x4ExtendHighI16x8SInst : public ImmNoneInst {
  public:
    I32x4ExtendHighI16x8SInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTEND_HIGH_I16x8_S) {}
};

class I32x4ExtendLowI16x8UInst : public ImmNoneInst {
  public:
    I32x4ExtendLowI16x8UInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTEND_LOW_I16x8_U) {}
};

class I32x4ExtendHighI16x8UInst : public ImmNoneInst {
  public:
    I32x4ExtendHighI16x8UInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTEND_HIGH_I16x8_U) {}
};

class I32x4ShlInst : public ImmNoneInst {
  public:
    I32x4ShlInst() : ImmNoneInst(WASM_OP_I32x4_SHL) {}
};

class I32x4ShrSInst : public ImmNoneInst {
  public:
    I32x4ShrSInst() : ImmNoneInst(WASM_OP_I32x4_SHR_S) {}
};

class I32x4ShrUInst : public ImmNoneInst {
  public:
    I32x4ShrUInst() : ImmNoneInst(WASM_OP_I32x4_SHR_U) {}
};

class I32x4AddInst : public ImmNoneInst {
  public:
    I32x4AddInst() : ImmNoneInst(WASM_OP_I32x4_ADD) {}
};

class I32x4SubInst : public ImmNoneInst {
  public:
    I32x4SubInst() : ImmNoneInst(WASM_OP_I32x4_SUB) {}
};

class I32x4MulInst : public ImmNoneInst {
  public:
    I32x4MulInst() : ImmNoneInst(WASM_OP_I32x4_MUL) {}
};

class I32x4MinSInst : public ImmNoneInst {
  public:
    I32x4MinSInst() : ImmNoneInst(WASM_OP_I32x4_MIN_S) {}
};

class I32x4MinUInst : public ImmNoneInst {
  public:
    I32x4MinUInst() : ImmNoneInst(WASM_OP_I32x4_MIN_U) {}
};

class I32x4MaxSInst : public ImmNoneInst {
  public:
    I32x4MaxSInst() : ImmNoneInst(WASM_OP_I32x4_MAX_S) {}
};

class I32x4MaxUInst : public ImmNoneInst {
  public:
    I32x4MaxUInst() : ImmNoneInst(WASM_OP_I32x4_MAX_U) {}
};

class I32x4DotI16x8SInst : public ImmNoneInst {
  public:
    I32x4DotI16x8SInst() : ImmNoneInst(WASM_OP_I32x4_DOT_I16x8_S) {}
};

class I32x4ExtmulLowI16x8SInst : public ImmNoneInst {
  public:
    I32x4ExtmulLowI16x8SInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTMUL_LOW_I16x8_S) {}
};

class I32x4ExtmulHighI16x8SInst : public ImmNoneInst {
  public:
    I32x4ExtmulHighI16x8SInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTMUL_HIGH_I16x8_S) {}
};

class I32x4ExtmulLowI16x8UInst : public ImmNoneInst {
  public:
    I32x4ExtmulLowI16x8UInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTMUL_LOW_I16x8_U) {}
};

class I32x4ExtmulHighI16x8UInst : public ImmNoneInst {
  public:
    I32x4ExtmulHighI16x8UInst()
        : ImmNoneInst(WASM_OP_I32x4_EXTMUL_HIGH_I16x8_U) {}
};

class I64x2AbsInst : public ImmNoneInst {
  public:
    I64x2AbsInst() : ImmNoneInst(WASM_OP_I64x2_ABS) {}
};

class I64x2NegInst : public ImmNoneInst {
  public:
    I64x2NegInst() : ImmNoneInst(WASM_OP_I64x2_NEG) {}
};

class I64x2AllTrueInst : public ImmNoneInst {
  public:
    I64x2AllTrueInst() : ImmNoneInst(WASM_OP_I64x2_ALL_TRUE) {}
};

class I64x2BitmaskInst : public ImmNoneInst {
  public:
    I64x2BitmaskInst() : ImmNoneInst(WASM_OP_I64x2_BITMASK) {}
};

class I64x2ExtendLowI32x4SInst : public ImmNoneInst {
  public:
    I64x2ExtendLowI32x4SInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTEND_LOW_I32x4_S) {}
};

class I64x2ExtendHighI32x4SInst : public ImmNoneInst {
  public:
    I64x2ExtendHighI32x4SInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTEND_HIGH_I32x4_S) {}
};

class I64x2ExtendLowI32x4UInst : public ImmNoneInst {
  public:
    I64x2ExtendLowI32x4UInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTEND_LOW_I32x4_U) {}
};

class I64x2ExtendHighI32x4UInst : public ImmNoneInst {
  public:
    I64x2ExtendHighI32x4UInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTEND_HIGH_I32x4_U) {}
};

class I64x2ShlInst : public ImmNoneInst {
  public:
    I64x2ShlInst() : ImmNoneInst(WASM_OP_I64x2_SHL) {}
};

class I64x2ShrSInst : public ImmNoneInst {
  public:
    I64x2ShrSInst() : ImmNoneInst(WASM_OP_I64x2_SHR_S) {}
};

class I64x2ShrUInst : public ImmNoneInst {
  public:
    I64x2ShrUInst() : ImmNoneInst(WASM_OP_I64x2_SHR_U) {}
};

class I64x2AddInst : public ImmNoneInst {
  public:
    I64x2AddInst() : ImmNoneInst(WASM_OP_I64x2_ADD) {}
};

class I64x2SubInst : public ImmNoneInst {
  public:
    I64x2SubInst() : ImmNoneInst(WASM_OP_I64x2_SUB) {}
};

class I64x2MulInst : public ImmNoneInst {
  public:
    I64x2MulInst() : ImmNoneInst(WASM_OP_I64x2_MUL) {}
};

class I64x2EqInst : public ImmNoneInst {
  public:
    I64x2EqInst() : ImmNoneInst(WASM_OP_I64x2_EQ) {}
};

class I64x2NeInst : public ImmNoneInst {
  public:
    I64x2NeInst() : ImmNoneInst(WASM_OP_I64x2_NE) {}
};

class I64x2LtSInst : public ImmNoneInst {
  public:
    I64x2LtSInst() : ImmNoneInst(WASM_OP_I64x2_LT_S) {}
};

class I64x2GtSInst : public ImmNoneInst {
  public:
    I64x2GtSInst() : ImmNoneInst(WASM_OP_I64x2_GT_S) {}
};

class I64x2LeSInst : public ImmNoneInst {
  public:
    I64x2LeSInst() : ImmNoneInst(WASM_OP_I64x2_LE_S) {}
};

class I64x2GeSInst : public ImmNoneInst {
  public:
    I64x2GeSInst() : ImmNoneInst(WASM_OP_I64x2_GE_S) {}
};

class I64x2ExtmulLowI32x4SInst : public ImmNoneInst {
  public:
    I64x2ExtmulLowI32x4SInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTMUL_LOW_I32x4_S) {}
};

class I64x2ExtmulHighI32x4SInst : public ImmNoneInst {
  public:
    I64x2ExtmulHighI32x4SInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTMUL_HIGH_I32x4_S) {}
};

class I64x2ExtmulLowI32x4UInst : public ImmNoneInst {
  public:
    I64x2ExtmulLowI32x4UInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTMUL_LOW_I32x4_U) {}
};

class I64x2ExtmulHighI32x4UInst : public ImmNoneInst {
  public:
    I64x2ExtmulHighI32x4UInst()
        : ImmNoneInst(WASM_OP_I64x2_EXTMUL_HIGH_I32x4_U) {}
};

class F32x4AbsInst : public ImmNoneInst {
  public:
    F32x4AbsInst() : ImmNoneInst(WASM_OP_F32x4_ABS) {}
};

class F32x4NegInst : public ImmNoneInst {
  public:
    F32x4NegInst() : ImmNoneInst(WASM_OP_F32x4_NEG) {}
};

class F32x4SqrtInst : public ImmNoneInst {
  public:
    F32x4SqrtInst() : ImmNoneInst(WASM_OP_F32x4_SQRT) {}
};

class F32x4AddInst : public ImmNoneInst {
  public:
    F32x4AddInst() : ImmNoneInst(WASM_OP_F32x4_ADD) {}
};

class F32x4SubInst : public ImmNoneInst {
  public:
    F32x4SubInst() : ImmNoneInst(WASM_OP_F32x4_SUB) {}
};

class F32x4MulInst : public ImmNoneInst {
  public:
    F32x4MulInst() : ImmNoneInst(WASM_OP_F32x4_MUL) {}
};

class F32x4DivInst : public ImmNoneInst {
  public:
    F32x4DivInst() : ImmNoneInst(WASM_OP_F32x4_DIV) {}
};

class F32x4MinInst : public ImmNoneInst {
  public:
    F32x4MinInst() : ImmNoneInst(WASM_OP_F32x4_MIN) {}
};

class F32x4MaxInst : public ImmNoneInst {
  public:
    F32x4MaxInst() : ImmNoneInst(WASM_OP_F32x4_MAX) {}
};

class F32x4PminInst : public ImmNoneInst {
  public:
    F32x4PminInst() : ImmNoneInst(WASM_OP_F32x4_PMIN) {}
};

class F32x4PmaxInst : public ImmNoneInst {
  public:
    F32x4PmaxInst() : ImmNoneInst(WASM_OP_F32x4_PMAX) {}
};

class F64x2AbsInst : public ImmNoneInst {
  public:
    F64x2AbsInst() : ImmNoneInst(WASM_OP_F64x2_ABS) {}
};

class F64x2NegInst : public ImmNoneInst {
  public:
    F64x2NegInst() : ImmNoneInst(WASM_OP_F64x2_NEG) {}
};

class F64x2SqrtInst : public ImmNoneInst {
  public:
    F64x2SqrtInst() : ImmNoneInst(WASM_OP_F64x2_SQRT) {}
};

class F64x2AddInst : public ImmNoneInst {
  public:
    F64x2AddInst() : ImmNoneInst(WASM_OP_F64x2_ADD) {}
};

class F64x2SubInst : public ImmNoneInst {
  public:
    F64x2SubInst() : ImmNoneInst(WASM_OP_F64x2_SUB) {}
};

class F64x2MulInst : public ImmNoneInst {
  public:
    F64x2MulInst() : ImmNoneInst(WASM_OP_F64x2_MUL) {}
};

class F64x2DivInst : public ImmNoneInst {
  public:
    F64x2DivInst() : ImmNoneInst(WASM_OP_F64x2_DIV) {}
};

class F64x2MinInst : public ImmNoneInst {
  public:
    F64x2MinInst() : ImmNoneInst(WASM_OP_F64x2_MIN) {}
};

class F64x2MaxInst : public ImmNoneInst {
  public:
    F64x2MaxInst() : ImmNoneInst(WASM_OP_F64x2_MAX) {}
};

class F64x2PminInst : public ImmNoneInst {
  public:
    F64x2PminInst() : ImmNoneInst(WASM_OP_F64x2_PMIN) {}
};

class F64x2PmaxInst : public ImmNoneInst {
  public:
    F64x2PmaxInst() : ImmNoneInst(WASM_OP_F64x2_PMAX) {}
};

class I32x4TruncSatF32x4SInst : public ImmNoneInst {
  public:
    I32x4TruncSatF32x4SInst() : ImmNoneInst(WASM_OP_I32x4_TRUNC_SAT_F32x4_S) {}
};

class I32x4TruncSatF32x4UInst : public ImmNoneInst {
  public:
    I32x4TruncSatF32x4UInst() : ImmNoneInst(WASM_OP_I32x4_TRUNC_SAT_F32x4_U) {}
};

class F32x4TruncSatI32x4SInst : public ImmNoneInst {
  public:
    F32x4TruncSatI32x4SInst() : ImmNoneInst(WASM_OP_F32x4_TRUNC_SAT_I32x4_S) {}
};

class F32x4TruncSatI32x4UInst : public ImmNoneInst {
  public:
    F32x4TruncSatI32x4UInst() : ImmNoneInst(WASM_OP_F32x4_TRUNC_SAT_I32x4_U) {}
};

class I32x4TruncSatF64x2SZeroInst : public ImmNoneInst {
  public:
    I32x4TruncSatF64x2SZeroInst()
        : ImmNoneInst(WASM_OP_I32x4_TRUNC_SAT_F64x2_S_ZERO) {}
};

class I32x4TruncSatF64x2UZeroInst : public ImmNoneInst {
  public:
    I32x4TruncSatF64x2UZeroInst()
        : ImmNoneInst(WASM_OP_I32x4_TRUNC_SAT_F64x2_U_ZERO) {}
};

class F64x2ConvertLowI32x4SInst : public ImmNoneInst {
  public:
    F64x2ConvertLowI32x4SInst()
        : ImmNoneInst(WASM_OP_F64x2_CONVERT_LOW_I32x4_S) {}
};

class F64x2ConvertLowI32x4UInst : public ImmNoneInst {
  public:
    F64x2ConvertLowI32x4UInst()
        : ImmNoneInst(WASM_OP_F64x2_CONVERT_LOW_I32x4_U) {}
};

class MemoryAtomicNotifyInst : public ImmMemargInst {
  public:
    MemoryAtomicNotifyInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_MEMORY_ATOMIC_NOTIFY, align, offset, mem) {}
};

class MemoryAtomicWait32Inst : public ImmMemargInst {
  public:
    MemoryAtomicWait32Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_MEMORY_ATOMIC_WAIT32, align, offset, mem) {}
};

class MemoryAtomicWait64Inst : public ImmMemargInst {
  public:
    MemoryAtomicWait64Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_MEMORY_ATOMIC_WAIT64, align, offset, mem) {}
};

class AtomicFenceInst : public ImmNoneInst {
  public:
    AtomicFenceInst() : ImmNoneInst(WASM_OP_ATOMIC_FENCE) {}
};

class I32AtomicLoadInst : public ImmMemargInst {
  public:
    I32AtomicLoadInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_LOAD, align, offset, mem) {}
};

class I64AtomicLoadInst : public ImmMemargInst {
  public:
    I64AtomicLoadInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_LOAD, align, offset, mem) {}
};

class I32AtomicLoad8UInst : public ImmMemargInst {
  public:
    I32AtomicLoad8UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_LOAD8_U, align, offset, mem) {}
};

class I32AtomicLoad16UInst : public ImmMemargInst {
  public:
    I32AtomicLoad16UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_LOAD16_U, align, offset, mem) {}
};

class I64AtomicLoad8UInst : public ImmMemargInst {
  public:
    I64AtomicLoad8UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_LOAD8_U, align, offset, mem) {}
};

class I64AtomicLoad16UInst : public ImmMemargInst {
  public:
    I64AtomicLoad16UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_LOAD16_U, align, offset, mem) {}
};

class I64AtomicLoad32UInst : public ImmMemargInst {
  public:
    I64AtomicLoad32UInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_LOAD32_U, align, offset, mem) {}
};

class I32AtomicStoreInst : public ImmMemargInst {
  public:
    I32AtomicStoreInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_STORE, align, offset, mem) {}
};

class I64AtomicStoreInst : public ImmMemargInst {
  public:
    I64AtomicStoreInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_STORE, align, offset, mem) {}
};

class I32AtomicStore8Inst : public ImmMemargInst {
  public:
    I32AtomicStore8Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_STORE8, align, offset, mem) {}
};

class I32AtomicStore16Inst : public ImmMemargInst {
  public:
    I32AtomicStore16Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_STORE16, align, offset, mem) {}
};

class I64AtomicStore8Inst : public ImmMemargInst {
  public:
    I64AtomicStore8Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_STORE8, align, offset, mem) {}
};

class I64AtomicStore16Inst : public ImmMemargInst {
  public:
    I64AtomicStore16Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_STORE16, align, offset, mem) {}
};

class I64AtomicStore32Inst : public ImmMemargInst {
  public:
    I64AtomicStore32Inst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_STORE32, align, offset, mem) {}
};

class I32AtomicRmwAddInst : public ImmMemargInst {
  public:
    I32AtomicRmwAddInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW_ADD, align, offset, mem) {}
};

class I64AtomicRmwAddInst : public ImmMemargInst {
  public:
    I64AtomicRmwAddInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW_ADD, align, offset, mem) {}
};

class I32AtomicRmw8AddUInst : public ImmMemargInst {
  public:
    I32AtomicRmw8AddUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW8_ADD_U, align, offset, mem) {}
};

class I32AtomicRmw16AddUInst : public ImmMemargInst {
  public:
    I32AtomicRmw16AddUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW16_ADD_U, align, offset, mem) {}
};

class I64AtomicRmw8AddUInst : public ImmMemargInst {
  public:
    I64AtomicRmw8AddUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW8_ADD_U, align, offset, mem) {}
};

class I64AtomicRmw16AddUInst : public ImmMemargInst {
  public:
    I64AtomicRmw16AddUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW16_ADD_U, align, offset, mem) {}
};

class I64AtomicRmw32AddUInst : public ImmMemargInst {
  public:
    I64AtomicRmw32AddUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW32_ADD_U, align, offset, mem) {}
};

class I32AtomicRmwSubInst : public ImmMemargInst {
  public:
    I32AtomicRmwSubInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW_SUB, align, offset, mem) {}
};

class I64AtomicRmwSubInst : public ImmMemargInst {
  public:
    I64AtomicRmwSubInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW_SUB, align, offset, mem) {}
};

class I32AtomicRmw8SubUInst : public ImmMemargInst {
  public:
    I32AtomicRmw8SubUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW8_SUB_U, align, offset, mem) {}
};

class I32AtomicRmw16SubUInst : public ImmMemargInst {
  public:
    I32AtomicRmw16SubUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW16_SUB_U, align, offset, mem) {}
};

class I64AtomicRmw8SubUInst : public ImmMemargInst {
  public:
    I64AtomicRmw8SubUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW8_SUB_U, align, offset, mem) {}
};

class I64AtomicRmw16SubUInst : public ImmMemargInst {
  public:
    I64AtomicRmw16SubUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW16_SUB_U, align, offset, mem) {}
};

class I64AtomicRmw32SubUInst : public ImmMemargInst {
  public:
    I64AtomicRmw32SubUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW32_SUB_U, align, offset, mem) {}
};

class I32AtomicRmwAndInst : public ImmMemargInst {
  public:
    I32AtomicRmwAndInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW_AND, align, offset, mem) {}
};

class I64AtomicRmwAndInst : public ImmMemargInst {
  public:
    I64AtomicRmwAndInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW_AND, align, offset, mem) {}
};

class I32AtomicRmw8AndUInst : public ImmMemargInst {
  public:
    I32AtomicRmw8AndUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW8_AND_U, align, offset, mem) {}
};

class I32AtomicRmw16AndUInst : public ImmMemargInst {
  public:
    I32AtomicRmw16AndUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW16_AND_U, align, offset, mem) {}
};

class I64AtomicRmw8AndUInst : public ImmMemargInst {
  public:
    I64AtomicRmw8AndUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW8_AND_U, align, offset, mem) {}
};

class I64AtomicRmw16AndUInst : public ImmMemargInst {
  public:
    I64AtomicRmw16AndUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW16_AND_U, align, offset, mem) {}
};

class I64AtomicRmw32AndUInst : public ImmMemargInst {
  public:
    I64AtomicRmw32AndUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW32_AND_U, align, offset, mem) {}
};

class I32AtomicRmwOrInst : public ImmMemargInst {
  public:
    I32AtomicRmwOrInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW_OR, align, offset, mem) {}
};

class I64AtomicRmwOrInst : public ImmMemargInst {
  public:
    I64AtomicRmwOrInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW_OR, align, offset, mem) {}
};

class I32AtomicRmw8OrUInst : public ImmMemargInst {
  public:
    I32AtomicRmw8OrUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW8_OR_U, align, offset, mem) {}
};

class I32AtomicRmw16OrUInst : public ImmMemargInst {
  public:
    I32AtomicRmw16OrUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW16_OR_U, align, offset, mem) {}
};

class I64AtomicRmw8OrUInst : public ImmMemargInst {
  public:
    I64AtomicRmw8OrUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW8_OR_U, align, offset, mem) {}
};

class I64AtomicRmw16OrUInst : public ImmMemargInst {
  public:
    I64AtomicRmw16OrUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW16_OR_U, align, offset, mem) {}
};

class I64AtomicRmw32OrUInst : public ImmMemargInst {
  public:
    I64AtomicRmw32OrUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW32_OR_U, align, offset, mem) {}
};

class I32AtomicRmwXorInst : public ImmMemargInst {
  public:
    I32AtomicRmwXorInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW_XOR, align, offset, mem) {}
};

class I64AtomicRmwXorInst : public ImmMemargInst {
  public:
    I64AtomicRmwXorInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW_XOR, align, offset, mem) {}
};

class I32AtomicRmw8XorUInst : public ImmMemargInst {
  public:
    I32AtomicRmw8XorUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW8_XOR_U, align, offset, mem) {}
};

class I32AtomicRmw16XorUInst : public ImmMemargInst {
  public:
    I32AtomicRmw16XorUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW16_XOR_U, align, offset, mem) {}
};

class I64AtomicRmw8XorUInst : public ImmMemargInst {
  public:
    I64AtomicRmw8XorUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW8_XOR_U, align, offset, mem) {}
};

class I64AtomicRmw16XorUInst : public ImmMemargInst {
  public:
    I64AtomicRmw16XorUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW16_XOR_U, align, offset, mem) {}
};

class I64AtomicRmw32XorUInst : public ImmMemargInst {
  public:
    I64AtomicRmw32XorUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW32_XOR_U, align, offset, mem) {}
};

class I32AtomicRmwXchgInst : public ImmMemargInst {
  public:
    I32AtomicRmwXchgInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW_XCHG, align, offset, mem) {}
};

class I64AtomicRmwXchgInst : public ImmMemargInst {
  public:
    I64AtomicRmwXchgInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW_XCHG, align, offset, mem) {}
};

class I32AtomicRmw8XchgUInst : public ImmMemargInst {
  public:
    I32AtomicRmw8XchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW8_XCHG_U, align, offset, mem) {}
};

class I32AtomicRmw16XchgUInst : public ImmMemargInst {
  public:
    I32AtomicRmw16XchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW16_XCHG_U, align, offset, mem) {}
};

class I64AtomicRmw8XchgUInst : public ImmMemargInst {
  public:
    I64AtomicRmw8XchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW8_XCHG_U, align, offset, mem) {}
};

class I64AtomicRmw16XchgUInst : public ImmMemargInst {
  public:
    I64AtomicRmw16XchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW16_XCHG_U, align, offset, mem) {}
};

class I64AtomicRmw32XchgUInst : public ImmMemargInst {
  public:
    I64AtomicRmw32XchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW32_XCHG_U, align, offset, mem) {}
};

class I32AtomicRmwCmpxchgInst : public ImmMemargInst {
  public:
    I32AtomicRmwCmpxchgInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW_CMPXCHG, align, offset, mem) {}
};

class I64AtomicRmwCmpxchgInst : public ImmMemargInst {
  public:
    I64AtomicRmwCmpxchgInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW_CMPXCHG, align, offset, mem) {}
};

class I32AtomicRmw8CmpxchgUInst : public ImmMemargInst {
  public:
    I32AtomicRmw8CmpxchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW8_CMPXCHG_U, align, offset, mem) {
    }
};

class I32AtomicRmw16CmpxchgUInst : public ImmMemargInst {
  public:
    I32AtomicRmw16CmpxchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I32_ATOMIC_RMW16_CMPXCHG_U, align, offset,
                        mem) {}
};

class I64AtomicRmw8CmpxchgUInst : public ImmMemargInst {
  public:
    I64AtomicRmw8CmpxchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW8_CMPXCHG_U, align, offset, mem) {
    }
};

class I64AtomicRmw16CmpxchgUInst : public ImmMemargInst {
  public:
    I64AtomicRmw16CmpxchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW16_CMPXCHG_U, align, offset,
                        mem) {}
};

class I64AtomicRmw32CmpxchgUInst : public ImmMemargInst {
  public:
    I64AtomicRmw32CmpxchgUInst(uint32_t align, uint32_t offset, MemoryDecl *mem)
        : ImmMemargInst(WASM_OP_I64_ATOMIC_RMW32_CMPXCHG_U, align, offset,
                        mem) {}
};
