#pragma once

#include <list>
#include <memory>
#include <functional>
#include "wasmdefs.h"

class InstBase;
// Instructions
typedef std::shared_ptr<InstBase> InstBasePtr;
typedef std::list<InstBasePtr>::iterator InstItr;
typedef std::list<InstBasePtr> InstList;

#define INST(inv) InstBasePtr(new inv)

// Scopes
struct ScopeBlock;
typedef std::list<ScopeBlock> ScopeList;

// Type lists
typedef std::list<wasm_type_t> TypeList;

class WasmModule;
class FuncDecl;
// Callbacks
typedef std::function<void(WasmModule &module, FuncDecl *src_func, FuncDecl* call_func)> DirectCallSiteCallback;
typedef std::function<void(WasmModule &module, FuncDecl *src_func, InstBasePtr instptr)> InstructionSiteCallback;
typedef std::function<void(WasmModule &module, FuncDecl *func)> FuncDeclCallback;

