#ifndef R3_MEMOPS_TABLE_H
#define R3_MEMOPS_TABLE_H

#include "wasmops.h"
#include "wasmdefs.h"

/* We cannot use SigDecl from C++ in this since we need array-initialization 
capability from C. Need to explicitly convert.. */
typedef struct {
	wasm_type_t params[3];
	wasm_type_t results[1];
} CSigDecl;

typedef enum {
	UNSIGNED,
	SIGNED
} Signed;

typedef struct {
	int64_t size;
	Signed sign;
	bool atomic;
	CSigDecl sig;
} MemopProp;

/* Signature declarations for all memory operations */
// One param
const CSigDecl sig_p_i32_r_i32 = {
	.params = { WASM_TYPE_I32 },
	.results = { WASM_TYPE_I32 }
};
const CSigDecl sig_p_i32_r_i64 = {
	.params = { WASM_TYPE_I32 },
	.results = { WASM_TYPE_I64 }
};
const CSigDecl sig_p_i32_r_f32 = {
	.params = { WASM_TYPE_I32 },
	.results = { WASM_TYPE_F32 }
};
const CSigDecl sig_p_i32_r_f64 = {
	.params = { WASM_TYPE_I32 },
	.results = { WASM_TYPE_F64 }
};

// Two params
const CSigDecl sig_p_i32x2_r_i32 = {
	.params = { WASM_TYPE_I32, WASM_TYPE_I32 },
	.results = { WASM_TYPE_I32 }
};
const CSigDecl sig_p_i32x2_r_void = {
	.params = { WASM_TYPE_I32, WASM_TYPE_I32 },
	.results = { }
};
const CSigDecl sig_p_i32i64_r_i64 = {
	.params = { WASM_TYPE_I32, WASM_TYPE_I64 },
	.results = { WASM_TYPE_I64 }
};
const CSigDecl sig_p_i32i64_r_void = {
	.params = { WASM_TYPE_I32, WASM_TYPE_I64 },
	.results = { }
};
const CSigDecl sig_p_i32f32_r_void = {
	.params = { WASM_TYPE_I32, WASM_TYPE_F32 },
	.results = { }
};
const CSigDecl sig_p_i32f64_r_void = {
	.params = { WASM_TYPE_I32, WASM_TYPE_F64 },
	.results = { }
};

// Three params
const CSigDecl sig_p_i32x3_r_i32 = {
	.params = { WASM_TYPE_I32, WASM_TYPE_I32, WASM_TYPE_I32 },
	.results = { WASM_TYPE_I32 }
};
const CSigDecl sig_p_i32x3_r_void = {
	.params = { WASM_TYPE_I32, WASM_TYPE_I32, WASM_TYPE_I32 },
	.results = { }
};
const CSigDecl sig_p_i32i64x2_r_i64 = {
	.params = { WASM_TYPE_I32, WASM_TYPE_I64, WASM_TYPE_I64 },
	.results = { WASM_TYPE_I64 }
};

extern MemopProp memop_inst_table[];
#endif