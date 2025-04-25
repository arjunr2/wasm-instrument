#include <sstream>

#include "routine_common.h"

// Instructions to typecast any type to I64
InstBuilder builder_push_i64_extend(InstBuilder &builder, wasm_type_t type) {
    switch (type) {
        case WASM_TYPE_I32: builder.push_inst (I64ExtendI32UInst()); break;
        case WASM_TYPE_I64: ; break;
        case WASM_TYPE_F32: builder.push({
            INST (I32ReinterpretF32Inst()),
            INST (I64ExtendI32UInst())
        }); break;
        case WASM_TYPE_F64: builder.push_inst (I64ReinterpretF64Inst()); break;
		default: { 
            ERR("Error: Unsupported type %d for I64 Extend\n", type); 
        }
    }
    return builder;
}

