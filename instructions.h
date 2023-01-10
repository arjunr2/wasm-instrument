#include "inst_internal.h"

class UnreachableInst: public ImmNoneInst {
	public:
	UnreachableInst() : ImmNoneInst(WASM_OP_UNREACHABLE) { }
};

class NopInst: public ImmNoneInst {
	public:
	NopInst() : ImmNoneInst(WASM_OP_NOP) { }
};

class BlockInst: public ImmBlocktInst {
	public:
	BlockInst(int64_t type) : ImmBlocktInst(WASM_OP_BLOCK, type) { }
};

class LoopInst: public ImmBlocktInst {
	public:
	LoopInst(int64_t type) : ImmBlocktInst(WASM_OP_LOOP, type) { }
};

class IfInst: public ImmBlocktInst {
	public:
	IfInst(int64_t type) : ImmBlocktInst(WASM_OP_IF, type) { }
};

class ElseInst: public ImmNoneInst {
	public:
	ElseInst() : ImmNoneInst(WASM_OP_ELSE) { }
};

class EndInst: public ImmNoneInst {
	public:
	EndInst() : ImmNoneInst(WASM_OP_END) { }
};

class BrInst: public ImmLabelInst {
	public:
	BrInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR, idx) { }
};

class BrIfInst: public ImmLabelInst {
	public:
	BrIfInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR_IF, idx) { }
};

class BrTableInst: public ImmLabelsInst {
	public:
	BrTableInst(std::list<uint32_t> idxs,uint32_t def_idx) : ImmLabelsInst(WASM_OP_BR_TABLE, idxs,def_idx) { }
};

class ReturnInst: public ImmNoneInst {
	public:
	ReturnInst() : ImmNoneInst(WASM_OP_RETURN) { }
};

class CallInst: public ImmFuncInst {
	public:
	CallInst(FuncDecl* func) : ImmFuncInst(WASM_OP_CALL, func) { }
};

class CallIndirectInst: public ImmSigTableInst {
	public:
	CallIndirectInst(SigDecl* sig,FuncDecl* func) : ImmSigTableInst(WASM_OP_CALL_INDIRECT, sig,func) { }
};

class DropInst: public ImmNoneInst {
	public:
	DropInst() : ImmNoneInst(WASM_OP_DROP) { }
};

class SelectInst: public ImmNoneInst {
	public:
	SelectInst() : ImmNoneInst(WASM_OP_SELECT) { }
};

class LocalGetInst: public ImmLocalInst {
	public:
	LocalGetInst(uint32_t idx) : ImmLocalInst(WASM_OP_LOCAL_GET, idx) { }
};

class LocalSetInst: public ImmLocalInst {
	public:
	LocalSetInst(uint32_t idx) : ImmLocalInst(WASM_OP_LOCAL_SET, idx) { }
};

class LocalTeeInst: public ImmLocalInst {
	public:
	LocalTeeInst(uint32_t idx) : ImmLocalInst(WASM_OP_LOCAL_TEE, idx) { }
};

class GlobalGetInst: public ImmGlobalInst {
	public:
	GlobalGetInst(GlobalDecl* global) : ImmGlobalInst(WASM_OP_GLOBAL_GET, global) { }
};

class GlobalSetInst: public ImmGlobalInst {
	public:
	GlobalSetInst(GlobalDecl* global) : ImmGlobalInst(WASM_OP_GLOBAL_SET, global) { }
};

class I64LoadInst: public ImmMemargInst {
	public:
	I64LoadInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_LOAD, align,offset) { }
};

class F32LoadInst: public ImmMemargInst {
	public:
	F32LoadInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_F32_LOAD, align,offset) { }
};

class I32LoadInst: public ImmMemargInst {
	public:
	I32LoadInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_LOAD, align,offset) { }
};

class F64LoadInst: public ImmMemargInst {
	public:
	F64LoadInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_F64_LOAD, align,offset) { }
};

class I32Load8SInst: public ImmMemargInst {
	public:
	I32Load8SInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_LOAD8_S, align,offset) { }
};

class I32Load8UInst: public ImmMemargInst {
	public:
	I32Load8UInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_LOAD8_U, align,offset) { }
};

class I32Load16SInst: public ImmMemargInst {
	public:
	I32Load16SInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_LOAD16_S, align,offset) { }
};

class I32Load16UInst: public ImmMemargInst {
	public:
	I32Load16UInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_LOAD16_U, align,offset) { }
};

class I64Load8SInst: public ImmMemargInst {
	public:
	I64Load8SInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_LOAD8_S, align,offset) { }
};

class I64Load8UInst: public ImmMemargInst {
	public:
	I64Load8UInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_LOAD8_U, align,offset) { }
};

class I64Load16SInst: public ImmMemargInst {
	public:
	I64Load16SInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_LOAD16_S, align,offset) { }
};

class I64Load16UInst: public ImmMemargInst {
	public:
	I64Load16UInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_LOAD16_U, align,offset) { }
};

class I64Load32SInst: public ImmMemargInst {
	public:
	I64Load32SInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_LOAD32_S, align,offset) { }
};

class I64Load32UInst: public ImmMemargInst {
	public:
	I64Load32UInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_LOAD32_U, align,offset) { }
};

class I32StoreInst: public ImmMemargInst {
	public:
	I32StoreInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_STORE, align,offset) { }
};

class I64StoreInst: public ImmMemargInst {
	public:
	I64StoreInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_STORE, align,offset) { }
};

class F32StoreInst: public ImmMemargInst {
	public:
	F32StoreInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_F32_STORE, align,offset) { }
};

class F64StoreInst: public ImmMemargInst {
	public:
	F64StoreInst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_F64_STORE, align,offset) { }
};

class I32Store8Inst: public ImmMemargInst {
	public:
	I32Store8Inst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_STORE8, align,offset) { }
};

class I32Store16Inst: public ImmMemargInst {
	public:
	I32Store16Inst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I32_STORE16, align,offset) { }
};

class I64Store8Inst: public ImmMemargInst {
	public:
	I64Store8Inst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_STORE8, align,offset) { }
};

class I64Store16Inst: public ImmMemargInst {
	public:
	I64Store16Inst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_STORE16, align,offset) { }
};

class I64Store32Inst: public ImmMemargInst {
	public:
	I64Store32Inst(uint32_t align,uint32_t offset) : ImmMemargInst(WASM_OP_I64_STORE32, align,offset) { }
};

class MemorySizeInst: public ImmMemoryInst {
	public:
	MemorySizeInst(MemoryDecl* mem) : ImmMemoryInst(WASM_OP_MEMORY_SIZE, mem) { }
};

class MemoryGrowInst: public ImmMemoryInst {
	public:
	MemoryGrowInst(MemoryDecl* mem) : ImmMemoryInst(WASM_OP_MEMORY_GROW, mem) { }
};

class I32ConstInst: public ImmI32Inst {
	public:
	I32ConstInst(uint32_t value) : ImmI32Inst(WASM_OP_I32_CONST, value) { }
};

class I64ConstInst: public ImmI64Inst {
	public:
	I64ConstInst(int64_t value) : ImmI64Inst(WASM_OP_I64_CONST, value) { }
};

class F32ConstInst: public ImmF32Inst {
	public:
	F32ConstInst(float value) : ImmF32Inst(WASM_OP_F32_CONST, value) { }
};

class F64ConstInst: public ImmF64Inst {
	public:
	F64ConstInst(double value) : ImmF64Inst(WASM_OP_F64_CONST, value) { }
};

class I32EqzInst: public ImmNoneInst {
	public:
	I32EqzInst() : ImmNoneInst(WASM_OP_I32_EQZ) { }
};

class I32EqInst: public ImmNoneInst {
	public:
	I32EqInst() : ImmNoneInst(WASM_OP_I32_EQ) { }
};

class I32NeInst: public ImmNoneInst {
	public:
	I32NeInst() : ImmNoneInst(WASM_OP_I32_NE) { }
};

class I32LtSInst: public ImmNoneInst {
	public:
	I32LtSInst() : ImmNoneInst(WASM_OP_I32_LT_S) { }
};

class I32LtUInst: public ImmNoneInst {
	public:
	I32LtUInst() : ImmNoneInst(WASM_OP_I32_LT_U) { }
};

class I32GtSInst: public ImmNoneInst {
	public:
	I32GtSInst() : ImmNoneInst(WASM_OP_I32_GT_S) { }
};

class I32GtUInst: public ImmNoneInst {
	public:
	I32GtUInst() : ImmNoneInst(WASM_OP_I32_GT_U) { }
};

class I32LeSInst: public ImmNoneInst {
	public:
	I32LeSInst() : ImmNoneInst(WASM_OP_I32_LE_S) { }
};

class I32LeUInst: public ImmNoneInst {
	public:
	I32LeUInst() : ImmNoneInst(WASM_OP_I32_LE_U) { }
};

class I32GeSInst: public ImmNoneInst {
	public:
	I32GeSInst() : ImmNoneInst(WASM_OP_I32_GE_S) { }
};

class I32GeUInst: public ImmNoneInst {
	public:
	I32GeUInst() : ImmNoneInst(WASM_OP_I32_GE_U) { }
};

class I64EqzInst: public ImmNoneInst {
	public:
	I64EqzInst() : ImmNoneInst(WASM_OP_I64_EQZ) { }
};

class I64EqInst: public ImmNoneInst {
	public:
	I64EqInst() : ImmNoneInst(WASM_OP_I64_EQ) { }
};

class I64NeInst: public ImmNoneInst {
	public:
	I64NeInst() : ImmNoneInst(WASM_OP_I64_NE) { }
};

class I64LtSInst: public ImmNoneInst {
	public:
	I64LtSInst() : ImmNoneInst(WASM_OP_I64_LT_S) { }
};

class I64LtUInst: public ImmNoneInst {
	public:
	I64LtUInst() : ImmNoneInst(WASM_OP_I64_LT_U) { }
};

class I64GtSInst: public ImmNoneInst {
	public:
	I64GtSInst() : ImmNoneInst(WASM_OP_I64_GT_S) { }
};

class I64GtUInst: public ImmNoneInst {
	public:
	I64GtUInst() : ImmNoneInst(WASM_OP_I64_GT_U) { }
};

class I64LeSInst: public ImmNoneInst {
	public:
	I64LeSInst() : ImmNoneInst(WASM_OP_I64_LE_S) { }
};

class I64LeUInst: public ImmNoneInst {
	public:
	I64LeUInst() : ImmNoneInst(WASM_OP_I64_LE_U) { }
};

class I64GeSInst: public ImmNoneInst {
	public:
	I64GeSInst() : ImmNoneInst(WASM_OP_I64_GE_S) { }
};

class I64GeUInst: public ImmNoneInst {
	public:
	I64GeUInst() : ImmNoneInst(WASM_OP_I64_GE_U) { }
};

class F32EqInst: public ImmNoneInst {
	public:
	F32EqInst() : ImmNoneInst(WASM_OP_F32_EQ) { }
};

class F32NeInst: public ImmNoneInst {
	public:
	F32NeInst() : ImmNoneInst(WASM_OP_F32_NE) { }
};

class F32LtInst: public ImmNoneInst {
	public:
	F32LtInst() : ImmNoneInst(WASM_OP_F32_LT) { }
};

class F32GtInst: public ImmNoneInst {
	public:
	F32GtInst() : ImmNoneInst(WASM_OP_F32_GT) { }
};

class F32LeInst: public ImmNoneInst {
	public:
	F32LeInst() : ImmNoneInst(WASM_OP_F32_LE) { }
};

class F32GeInst: public ImmNoneInst {
	public:
	F32GeInst() : ImmNoneInst(WASM_OP_F32_GE) { }
};

class F64EqInst: public ImmNoneInst {
	public:
	F64EqInst() : ImmNoneInst(WASM_OP_F64_EQ) { }
};

class F64NeInst: public ImmNoneInst {
	public:
	F64NeInst() : ImmNoneInst(WASM_OP_F64_NE) { }
};

class F64LtInst: public ImmNoneInst {
	public:
	F64LtInst() : ImmNoneInst(WASM_OP_F64_LT) { }
};

class F64GtInst: public ImmNoneInst {
	public:
	F64GtInst() : ImmNoneInst(WASM_OP_F64_GT) { }
};

class F64LeInst: public ImmNoneInst {
	public:
	F64LeInst() : ImmNoneInst(WASM_OP_F64_LE) { }
};

class F64GeInst: public ImmNoneInst {
	public:
	F64GeInst() : ImmNoneInst(WASM_OP_F64_GE) { }
};

class I32ClzInst: public ImmNoneInst {
	public:
	I32ClzInst() : ImmNoneInst(WASM_OP_I32_CLZ) { }
};

class I32CtzInst: public ImmNoneInst {
	public:
	I32CtzInst() : ImmNoneInst(WASM_OP_I32_CTZ) { }
};

class I32PopcntInst: public ImmNoneInst {
	public:
	I32PopcntInst() : ImmNoneInst(WASM_OP_I32_POPCNT) { }
};

class I32AddInst: public ImmNoneInst {
	public:
	I32AddInst() : ImmNoneInst(WASM_OP_I32_ADD) { }
};

class I32SubInst: public ImmNoneInst {
	public:
	I32SubInst() : ImmNoneInst(WASM_OP_I32_SUB) { }
};

class I32MulInst: public ImmNoneInst {
	public:
	I32MulInst() : ImmNoneInst(WASM_OP_I32_MUL) { }
};

class I32DivSInst: public ImmNoneInst {
	public:
	I32DivSInst() : ImmNoneInst(WASM_OP_I32_DIV_S) { }
};

class I32DivUInst: public ImmNoneInst {
	public:
	I32DivUInst() : ImmNoneInst(WASM_OP_I32_DIV_U) { }
};

class I32RemSInst: public ImmNoneInst {
	public:
	I32RemSInst() : ImmNoneInst(WASM_OP_I32_REM_S) { }
};

class I32RemUInst: public ImmNoneInst {
	public:
	I32RemUInst() : ImmNoneInst(WASM_OP_I32_REM_U) { }
};

class I32AndInst: public ImmNoneInst {
	public:
	I32AndInst() : ImmNoneInst(WASM_OP_I32_AND) { }
};

class I32OrInst: public ImmNoneInst {
	public:
	I32OrInst() : ImmNoneInst(WASM_OP_I32_OR) { }
};

class I32XorInst: public ImmNoneInst {
	public:
	I32XorInst() : ImmNoneInst(WASM_OP_I32_XOR) { }
};

class I32ShlInst: public ImmNoneInst {
	public:
	I32ShlInst() : ImmNoneInst(WASM_OP_I32_SHL) { }
};

class I32ShrSInst: public ImmNoneInst {
	public:
	I32ShrSInst() : ImmNoneInst(WASM_OP_I32_SHR_S) { }
};

class I32ShrUInst: public ImmNoneInst {
	public:
	I32ShrUInst() : ImmNoneInst(WASM_OP_I32_SHR_U) { }
};

class I32RotlInst: public ImmNoneInst {
	public:
	I32RotlInst() : ImmNoneInst(WASM_OP_I32_ROTL) { }
};

class I32RotrInst: public ImmNoneInst {
	public:
	I32RotrInst() : ImmNoneInst(WASM_OP_I32_ROTR) { }
};

class I64ClzInst: public ImmNoneInst {
	public:
	I64ClzInst() : ImmNoneInst(WASM_OP_I64_CLZ) { }
};

class I64CtzInst: public ImmNoneInst {
	public:
	I64CtzInst() : ImmNoneInst(WASM_OP_I64_CTZ) { }
};

class I64PopcntInst: public ImmNoneInst {
	public:
	I64PopcntInst() : ImmNoneInst(WASM_OP_I64_POPCNT) { }
};

class I64AddInst: public ImmNoneInst {
	public:
	I64AddInst() : ImmNoneInst(WASM_OP_I64_ADD) { }
};

class I64SubInst: public ImmNoneInst {
	public:
	I64SubInst() : ImmNoneInst(WASM_OP_I64_SUB) { }
};

class I64MulInst: public ImmNoneInst {
	public:
	I64MulInst() : ImmNoneInst(WASM_OP_I64_MUL) { }
};

class I64DivSInst: public ImmNoneInst {
	public:
	I64DivSInst() : ImmNoneInst(WASM_OP_I64_DIV_S) { }
};

class I64DivUInst: public ImmNoneInst {
	public:
	I64DivUInst() : ImmNoneInst(WASM_OP_I64_DIV_U) { }
};

class I64RemSInst: public ImmNoneInst {
	public:
	I64RemSInst() : ImmNoneInst(WASM_OP_I64_REM_S) { }
};

class I64RemUInst: public ImmNoneInst {
	public:
	I64RemUInst() : ImmNoneInst(WASM_OP_I64_REM_U) { }
};

class I64AndInst: public ImmNoneInst {
	public:
	I64AndInst() : ImmNoneInst(WASM_OP_I64_AND) { }
};

class I64OrInst: public ImmNoneInst {
	public:
	I64OrInst() : ImmNoneInst(WASM_OP_I64_OR) { }
};

class I64XorInst: public ImmNoneInst {
	public:
	I64XorInst() : ImmNoneInst(WASM_OP_I64_XOR) { }
};

class I64ShlInst: public ImmNoneInst {
	public:
	I64ShlInst() : ImmNoneInst(WASM_OP_I64_SHL) { }
};

class I64ShrSInst: public ImmNoneInst {
	public:
	I64ShrSInst() : ImmNoneInst(WASM_OP_I64_SHR_S) { }
};

class I64ShrUInst: public ImmNoneInst {
	public:
	I64ShrUInst() : ImmNoneInst(WASM_OP_I64_SHR_U) { }
};

class I64RotlInst: public ImmNoneInst {
	public:
	I64RotlInst() : ImmNoneInst(WASM_OP_I64_ROTL) { }
};

class I64RotrInst: public ImmNoneInst {
	public:
	I64RotrInst() : ImmNoneInst(WASM_OP_I64_ROTR) { }
};

class F32AbsInst: public ImmNoneInst {
	public:
	F32AbsInst() : ImmNoneInst(WASM_OP_F32_ABS) { }
};

class F32NegInst: public ImmNoneInst {
	public:
	F32NegInst() : ImmNoneInst(WASM_OP_F32_NEG) { }
};

class F32CeilInst: public ImmNoneInst {
	public:
	F32CeilInst() : ImmNoneInst(WASM_OP_F32_CEIL) { }
};

class F32FloorInst: public ImmNoneInst {
	public:
	F32FloorInst() : ImmNoneInst(WASM_OP_F32_FLOOR) { }
};

class F32TruncInst: public ImmNoneInst {
	public:
	F32TruncInst() : ImmNoneInst(WASM_OP_F32_TRUNC) { }
};

class F32NearestInst: public ImmNoneInst {
	public:
	F32NearestInst() : ImmNoneInst(WASM_OP_F32_NEAREST) { }
};

class F32SqrtInst: public ImmNoneInst {
	public:
	F32SqrtInst() : ImmNoneInst(WASM_OP_F32_SQRT) { }
};

class F32AddInst: public ImmNoneInst {
	public:
	F32AddInst() : ImmNoneInst(WASM_OP_F32_ADD) { }
};

class F32SubInst: public ImmNoneInst {
	public:
	F32SubInst() : ImmNoneInst(WASM_OP_F32_SUB) { }
};

class F32MulInst: public ImmNoneInst {
	public:
	F32MulInst() : ImmNoneInst(WASM_OP_F32_MUL) { }
};

class F32DivInst: public ImmNoneInst {
	public:
	F32DivInst() : ImmNoneInst(WASM_OP_F32_DIV) { }
};

class F32MinInst: public ImmNoneInst {
	public:
	F32MinInst() : ImmNoneInst(WASM_OP_F32_MIN) { }
};

class F32MaxInst: public ImmNoneInst {
	public:
	F32MaxInst() : ImmNoneInst(WASM_OP_F32_MAX) { }
};

class F32CopysignInst: public ImmNoneInst {
	public:
	F32CopysignInst() : ImmNoneInst(WASM_OP_F32_COPYSIGN) { }
};

class F64AbsInst: public ImmNoneInst {
	public:
	F64AbsInst() : ImmNoneInst(WASM_OP_F64_ABS) { }
};

class F64NegInst: public ImmNoneInst {
	public:
	F64NegInst() : ImmNoneInst(WASM_OP_F64_NEG) { }
};

class F64CeilInst: public ImmNoneInst {
	public:
	F64CeilInst() : ImmNoneInst(WASM_OP_F64_CEIL) { }
};

class F64FloorInst: public ImmNoneInst {
	public:
	F64FloorInst() : ImmNoneInst(WASM_OP_F64_FLOOR) { }
};

class F64TruncInst: public ImmNoneInst {
	public:
	F64TruncInst() : ImmNoneInst(WASM_OP_F64_TRUNC) { }
};

class F64NearestInst: public ImmNoneInst {
	public:
	F64NearestInst() : ImmNoneInst(WASM_OP_F64_NEAREST) { }
};

class F64SqrtInst: public ImmNoneInst {
	public:
	F64SqrtInst() : ImmNoneInst(WASM_OP_F64_SQRT) { }
};

class F64AddInst: public ImmNoneInst {
	public:
	F64AddInst() : ImmNoneInst(WASM_OP_F64_ADD) { }
};

class F64SubInst: public ImmNoneInst {
	public:
	F64SubInst() : ImmNoneInst(WASM_OP_F64_SUB) { }
};

class F64MulInst: public ImmNoneInst {
	public:
	F64MulInst() : ImmNoneInst(WASM_OP_F64_MUL) { }
};

class F64DivInst: public ImmNoneInst {
	public:
	F64DivInst() : ImmNoneInst(WASM_OP_F64_DIV) { }
};

class F64MinInst: public ImmNoneInst {
	public:
	F64MinInst() : ImmNoneInst(WASM_OP_F64_MIN) { }
};

class F64MaxInst: public ImmNoneInst {
	public:
	F64MaxInst() : ImmNoneInst(WASM_OP_F64_MAX) { }
};

class F64CopysignInst: public ImmNoneInst {
	public:
	F64CopysignInst() : ImmNoneInst(WASM_OP_F64_COPYSIGN) { }
};

class I32WrapI64Inst: public ImmNoneInst {
	public:
	I32WrapI64Inst() : ImmNoneInst(WASM_OP_I32_WRAP_I64) { }
};

class I32TruncF32SInst: public ImmNoneInst {
	public:
	I32TruncF32SInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F32_S) { }
};

class I32TruncF32UInst: public ImmNoneInst {
	public:
	I32TruncF32UInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F32_U) { }
};

class I32TruncF64SInst: public ImmNoneInst {
	public:
	I32TruncF64SInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F64_S) { }
};

class I32TruncF64UInst: public ImmNoneInst {
	public:
	I32TruncF64UInst() : ImmNoneInst(WASM_OP_I32_TRUNC_F64_U) { }
};

class I64ExtendI32SInst: public ImmNoneInst {
	public:
	I64ExtendI32SInst() : ImmNoneInst(WASM_OP_I64_EXTEND_I32_S) { }
};

class I64ExtendI32UInst: public ImmNoneInst {
	public:
	I64ExtendI32UInst() : ImmNoneInst(WASM_OP_I64_EXTEND_I32_U) { }
};

class I64TruncF32SInst: public ImmNoneInst {
	public:
	I64TruncF32SInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F32_S) { }
};

class I64TruncF32UInst: public ImmNoneInst {
	public:
	I64TruncF32UInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F32_U) { }
};

class I64TruncF64SInst: public ImmNoneInst {
	public:
	I64TruncF64SInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F64_S) { }
};

class I64TruncF64UInst: public ImmNoneInst {
	public:
	I64TruncF64UInst() : ImmNoneInst(WASM_OP_I64_TRUNC_F64_U) { }
};

class F32ConvertI32SInst: public ImmNoneInst {
	public:
	F32ConvertI32SInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I32_S) { }
};

class F32ConvertI32UInst: public ImmNoneInst {
	public:
	F32ConvertI32UInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I32_U) { }
};

class F32ConvertI64SInst: public ImmNoneInst {
	public:
	F32ConvertI64SInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I64_S) { }
};

class F32ConvertI64UInst: public ImmNoneInst {
	public:
	F32ConvertI64UInst() : ImmNoneInst(WASM_OP_F32_CONVERT_I64_U) { }
};

class F32DemoteF64Inst: public ImmNoneInst {
	public:
	F32DemoteF64Inst() : ImmNoneInst(WASM_OP_F32_DEMOTE_F64) { }
};

class F64ConvertI32SInst: public ImmNoneInst {
	public:
	F64ConvertI32SInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I32_S) { }
};

class F64ConvertI32UInst: public ImmNoneInst {
	public:
	F64ConvertI32UInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I32_U) { }
};

class F64ConvertI64SInst: public ImmNoneInst {
	public:
	F64ConvertI64SInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I64_S) { }
};

class F64ConvertI64UInst: public ImmNoneInst {
	public:
	F64ConvertI64UInst() : ImmNoneInst(WASM_OP_F64_CONVERT_I64_U) { }
};

class F64PromoteF32Inst: public ImmNoneInst {
	public:
	F64PromoteF32Inst() : ImmNoneInst(WASM_OP_F64_PROMOTE_F32) { }
};

class I32ReinterpretF32Inst: public ImmNoneInst {
	public:
	I32ReinterpretF32Inst() : ImmNoneInst(WASM_OP_I32_REINTERPRET_F32) { }
};

class I64ReinterpretF64Inst: public ImmNoneInst {
	public:
	I64ReinterpretF64Inst() : ImmNoneInst(WASM_OP_I64_REINTERPRET_F64) { }
};

class F32ReinterpretI32Inst: public ImmNoneInst {
	public:
	F32ReinterpretI32Inst() : ImmNoneInst(WASM_OP_F32_REINTERPRET_I32) { }
};

class F64ReinterpretI64Inst: public ImmNoneInst {
	public:
	F64ReinterpretI64Inst() : ImmNoneInst(WASM_OP_F64_REINTERPRET_I64) { }
};

class TryInst: public ImmBlocktInst {
	public:
	TryInst(int64_t type) : ImmBlocktInst(WASM_OP_TRY, type) { }
};

class CatchInst: public ImmTagInst {
	public:
	CatchInst() : ImmTagInst(WASM_OP_CATCH) { }
};

class ThrowInst: public ImmTagInst {
	public:
	ThrowInst() : ImmTagInst(WASM_OP_THROW) { }
};

class RethrowInst: public ImmNoneInst {
	public:
	RethrowInst() : ImmNoneInst(WASM_OP_RETHROW) { }
};

class ReturnCallInst: public ImmFuncInst {
	public:
	ReturnCallInst(FuncDecl* func) : ImmFuncInst(WASM_OP_RETURN_CALL, func) { }
};

class ReturnCallIndirectInst: public ImmSigTableInst {
	public:
	ReturnCallIndirectInst(SigDecl* sig,FuncDecl* func) : ImmSigTableInst(WASM_OP_RETURN_CALL_INDIRECT, sig,func) { }
};

class CallRefInst: public ImmNoneInst {
	public:
	CallRefInst() : ImmNoneInst(WASM_OP_CALL_REF) { }
};

class ReturnCallRefInst: public ImmNoneInst {
	public:
	ReturnCallRefInst() : ImmNoneInst(WASM_OP_RETURN_CALL_REF) { }
};

class DelegateInst: public ImmNoneInst {
	public:
	DelegateInst() : ImmNoneInst(WASM_OP_DELEGATE) { }
};

class CatchAllInst: public ImmNoneInst {
	public:
	CatchAllInst() : ImmNoneInst(WASM_OP_CATCH_ALL) { }
};

class SelectTInst: public ImmValtsInst {
	public:
	SelectTInst(std::list<wasm_type_t> types) : ImmValtsInst(WASM_OP_SELECT_T, types) { }
};

class TableGetInst: public ImmTableInst {
	public:
	TableGetInst(TableDecl* table) : ImmTableInst(WASM_OP_TABLE_GET, table) { }
};

class TableSetInst: public ImmTableInst {
	public:
	TableSetInst(TableDecl* table) : ImmTableInst(WASM_OP_TABLE_SET, table) { }
};

class I32Extend8SInst: public ImmNoneInst {
	public:
	I32Extend8SInst() : ImmNoneInst(WASM_OP_I32_EXTEND8_S) { }
};

class I32Extend16SInst: public ImmNoneInst {
	public:
	I32Extend16SInst() : ImmNoneInst(WASM_OP_I32_EXTEND16_S) { }
};

class I64Extend8SInst: public ImmNoneInst {
	public:
	I64Extend8SInst() : ImmNoneInst(WASM_OP_I64_EXTEND8_S) { }
};

class I64Extend16SInst: public ImmNoneInst {
	public:
	I64Extend16SInst() : ImmNoneInst(WASM_OP_I64_EXTEND16_S) { }
};

class I64Extend32SInst: public ImmNoneInst {
	public:
	I64Extend32SInst() : ImmNoneInst(WASM_OP_I64_EXTEND32_S) { }
};

class RefNullInst: public ImmRefnulltInst {
	public:
	RefNullInst(wasm_type_t type) : ImmRefnulltInst(WASM_OP_REF_NULL, type) { }
};

class RefIsNullInst: public ImmNoneInst {
	public:
	RefIsNullInst() : ImmNoneInst(WASM_OP_REF_IS_NULL) { }
};

class RefFuncInst: public ImmFuncInst {
	public:
	RefFuncInst(FuncDecl* func) : ImmFuncInst(WASM_OP_REF_FUNC, func) { }
};

class RefAsNonNullInst: public ImmNoneInst {
	public:
	RefAsNonNullInst() : ImmNoneInst(WASM_OP_REF_AS_NON_NULL) { }
};

class BrOnNullInst: public ImmLabelInst {
	public:
	BrOnNullInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR_ON_NULL, idx) { }
};

class RefEqInst: public ImmNoneInst {
	public:
	RefEqInst() : ImmNoneInst(WASM_OP_REF_EQ) { }
};

class BrOnNonNullInst: public ImmLabelInst {
	public:
	BrOnNonNullInst(uint32_t idx) : ImmLabelInst(WASM_OP_BR_ON_NON_NULL, idx) { }
};

