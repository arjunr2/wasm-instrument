# Webassembly Static Instrumenter

A convenient library to read and freely instrument WASM code statically. 
The repository is mostly up-to-date with the latest WASM standards -- including support for 
threading/atomics and bulk memory instructions

## Sample instrumentation

Examples of routines can be found in [routines.cpp](routines.cpp). Support for more complex views is still in progress, but currently implemented APIs can be found in 
[views.cpp](views.cpp)

The `sample_instrument` method below covers basic usage of the APIs. 
You only need to import `ir.h` and `instructions.h` for basic functionality.

```cpp
#include "ir.h"
#include "instructions.h"

void sample_instrument (WasmModule& module) {
  /* Global inmodule */
  GlobalDecl global = { 
    .type = WASM_TYPE_I32, 
    .is_mutable = true,
    .init_expr_bytes = INIT_EXPR (I32_CONST, 5)
  };
  module.add_global(global, "inmodule_global");

  /* Global import */
  ImportInfo iminfo = {
    .mod_name = "instrumentest",
    .member_name = "newglob"
  };
  GlobalInfo imglob = {
    .type = WASM_TYPE_EXTERNREF,
    .is_mutable = false
  };
  module.add_import(iminfo, imglob);

  /* Function import */
  iminfo.member_name = "newfunc";
  SigDecl imfunc = {
    .params = {WASM_TYPE_I32},
    .results = {WASM_TYPE_F64}
  };
  ImportDecl* func_imp = module.add_import(iminfo, imfunc);

  /* Export find */
  ExportDecl* exp = module.find_export("printf");
  if (exp == NULL) {
    TRACE("Printf not found\n");
  }

  /* Add a I32 Const + Drop before every call in main */
  ExportDecl* main_exp = module.find_export("main");
  FuncDecl* main_fn = main_exp->desc.func;
  InstList &insts = main_fn->instructions;
  for (auto institr = insts.begin(); institr != insts.end(); ++institr) {
    InstBasePtr &instruction = *institr;
    if (instruction->is(WASM_OP_CALL)) {
      insts.insert(institr, InstBasePtr(new I32ConstInst(0xDEADBEEF)));
      insts.insert(institr, InstBasePtr(new DropInst()));
    }
  }
}
```


## API Methods

Webassembly has the following index spaces -- **Global**, **Table**, **Memory**, **Func**. The folllowing specifications
will use *<ispace>* to denote of these. Refer to [ir.h](ir.h) for details

### Addition Operations

#### Add to Index Space
```cpp
<ispace>Decl* add_<ispace> (<ispace>Decl &decl, const char* export_name = NULL);
```

#### Add Import
```cpp
ImportDecl* add_import (ImportInfo &info, <ispace>Decl &decl);
```

#### Add Export
```cpp
ExportDecl* add_export (std::string export_name, <ispace>Decl &var);
```

### Find Operations

#### Find Export
```cpp
ExportDecl* find_export (std::string export_name);
```


### Working with instructions

Each function has an `InstList` object to iterate over instructions. Since it uses a standard list, 
all instructions iterators are preserved when inserting/removing from the structure. All supported instructions
and their constructors can be found [here](instructions.h).



