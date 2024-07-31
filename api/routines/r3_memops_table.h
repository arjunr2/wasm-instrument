#ifndef R3_MEMOPS_TABLE_H
#define R3_MEMOPS_TABLE_H

#include "wasmops.h"

typedef enum {
	UNSIGNED,
	SIGNED
} Signed;

typedef struct {
	int64_t size;
	Signed sign;
	bool atomic;
} MemopProp;

extern MemopProp memop_inst_table[];
#endif