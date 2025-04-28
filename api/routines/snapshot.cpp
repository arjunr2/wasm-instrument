#include "routine_common.h"

// Insert snapshot method on program exit
void snapshot_exit_insert(WasmModule &module, MemoryDecl *memory,
                          uint32_t instmem_start) {
    SigDecl s = {.params = {WASM_TYPE_I32}, .results = {}};
    ImportInfo iminfo = {.mod_name = "env", .member_name = "snapshot"};
    ImportDecl *import_decl = module.add_import(iminfo, s);

    InstBuilder builder = {};
    // Store global state into memory
    for (uint32_t i = 0; i < module.Globals().size(); i++) {
        GlobalDecl *global = module.getGlobal(i);
        if (global->is_mutable) {
            builder
                .push({
                    INST(I32ConstInst(0)),
                    INST(GlobalGetInst(global)),
                })
                .i64_convert(global->type)
                .push_inst(I64StoreInst(0, instmem_start, module.getMemory(0)));
        }
        instmem_start += 8;
    }
    builder.push({
        INST(MemorySizeInst(memory)),
        INST(I32ConstInst(WASM_PAGE_SIZE)),
        INST(I32MulInst()),
        INST(CallInst(import_decl->desc.func)),
    });

    std::vector<ImportInfo> exit_imports{
        {.mod_name = "wasi_snapshot_preview1", .member_name = "proc_exit"}};
    module.exit_instrument(builder, exit_imports);
}

void open_callback(WasmModule &module, FuncDecl *src_func,
                   FuncDecl *call_func) {
    TypeList x = call_func->sig->params;
    printf("Callback called\n");
}

void open_insert(WasmModule &module) {
    ImportInfo open_info = {.mod_name = "wasi_snapshot_preview1",
                            .member_name = "path_open"};
    FuncDecl *open_fn =
        module.find_import_func(open_info.mod_name, open_info.member_name);
    if (open_fn == NULL) {
        return;
    }

    std::map<FuncDecl *, DirectCallSiteCallback> openmap = {
        {open_fn, open_callback}};

    module.beforeFuncCall(openmap);
}

void snapshot_instrument(WasmModule &module) {

    MemoryDecl *memory = module.getMemory(0);

    // Add pages for global variables
    uint32_t num_globals = module.Globals().size();
    uint32_t globals_per_page = WASM_PAGE_SIZE / 8;

    uint32_t num_additional_pages =
        ((num_globals + globals_per_page - 1) / globals_per_page);
    uint32_t old_pages = memory->add_pages(1);
    uint32_t instmem_start = old_pages * WASM_PAGE_SIZE;

    snapshot_exit_insert(module, memory, instmem_start);
    open_insert(module);
}
