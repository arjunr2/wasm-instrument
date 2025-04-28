#include <cstring>
#include "builder.h"
#include "ir_defs.h"
#include "instructions.h"

InstBuilder& InstBuilder::i64_convert(wasm_type_t type) {
    switch (type) {
        case WASM_TYPE_I32: this->push_inst (I64ExtendI32UInst()); break;
        case WASM_TYPE_I64: ; break;
        case WASM_TYPE_F32: this->push({
            INST (I32ReinterpretF32Inst()),
            INST (I64ExtendI32UInst())
        }); break;
        case WASM_TYPE_F64: this->push_inst (I64ReinterpretF64Inst()); break;
		default: { 
            ERR("Error: Unsupported type %d for I64 Extend\n", type); 
        }
    }
    return *this;
}


InstBuilder& InstBuilder::ne(wasm_type_t type) {
    auto neq = [&type] {
        switch (type) {
            case WASM_TYPE_I32: return INST(I32NeInst());
            case WASM_TYPE_I64: return INST(I64NeInst());
            case WASM_TYPE_F32: return INST(F32NeInst());
            case WASM_TYPE_F64: return INST(F64NeInst());
            default: { 
                ERR("Error: Unsupported type %d for NEQ\n", type); 
                return (InstBasePtr) NULL; 
            }
        }
    }();
    if (neq != NULL) {
        this->push_inst(neq);
    }
    return *this;
}

InstBuilder& InstBuilder::i64_store(uint32_t size, MemoryDecl *mem) {
    auto store = [&size, &mem] {
        switch (size) {
            case 1: return INST(I64Store8Inst(0, 0, mem));
            case 2: return INST(I64Store16Inst(0, 0, mem));
            case 4: return INST(I64Store32Inst(0, 0, mem));
            case 8: return INST(I64StoreInst(0, 0, mem));
            default: { 
                ERR("Error: Unsupported size %d for writeback\n", size); 
                return (InstBasePtr) NULL;
            }
        }
    }();
    if (store != NULL) {
        this->push_inst(store);
    }
    return *this;
}

InstBuilder& InstBuilder::scalar_const(wasm_type_t type, int64_t val64) {
    uint32_t val32;
    float valf32;
    double valf64;
    auto store = [&type, &val64, &val32, &valf32, &valf64] {
        switch (type) {
            case WASM_TYPE_I32: {
                std::memcpy(&val32, &val64, sizeof(uint32_t));
                return INST(I32ConstInst(val32));
            }
            case WASM_TYPE_I64: {
                return INST(I64ConstInst(val64));
            }
            case WASM_TYPE_F32: {
                std::memcpy(&valf32, &val64, sizeof(float));
                return INST(F32ConstInst(valf32));
            }
            case WASM_TYPE_F64: {
                std::memcpy(&valf64, &val64, sizeof(double));
                return INST(F64ConstInst(valf64));
            }
            default: {
                ERR("Invalid type %d for return value\n", type);
                return (InstBasePtr) NULL;
            }
        }
    }();
    if (store != NULL) {
        this->push_inst(store);
    }
    return *this;
}

