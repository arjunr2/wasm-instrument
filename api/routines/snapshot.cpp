#include "routine_common.h"

#define INST(inv) InstBasePtr(new inv)

void snapshot_instrument (WasmModule &module) {
    SigDecl s = { .params = { WASM_TYPE_I32 }, .results = {  } };
    ImportInfo iminfo = { .mod_name = "env", .member_name = "snapshot" };
    ImportDecl* import_decl = module.add_import(iminfo, s); 

    FuncDecl *main_fn = get_main_export(module)->desc.func;
    MemoryDecl *memory = module.getMemory(0);

    // Add pages for global variables
    uint32_t num_globals = module.Globals().size();
    uint32_t globals_per_page = WASM_PAGE_SIZE / 8;


    uint32_t num_additional_pages = ((num_globals + globals_per_page - 1) / globals_per_page);
    uint32_t old_pages = add_pages(memory, 1);
    uint32_t instmem_start = old_pages * WASM_PAGE_SIZE;
    
    InstBuilder builder = {};
    // Store global state into memory
    for (uint32_t i = 0; i < module.Globals().size(); i++) {
        GlobalDecl *global = module.getGlobal(i);
        if (global->is_mutable) {
            builder.push({
                INST(I32ConstInst(0)),
                INST(GlobalGetInst(global)),
            });
            builder_push_i64_extend(builder, global->type);
            builder.push_inst(I64StoreInst(0, instmem_start, module.getMemory(0)));
        }
        instmem_start += 8;
    }
    builder.push({
        INST(MemorySizeInst(memory)),
        INST(I32ConstInst(globals_per_page)),
        INST(I32MulInst()),
        INST(CallInst(import_decl->desc.func)),
    });
    

    std::vector<ImportInfo> exit_imports {
        {
            .mod_name = "wasi_snapshot_preview1",
            .member_name = "proc_exit"
        }
    };
    insert_on_exit(module, main_fn, builder, exit_imports);
}
